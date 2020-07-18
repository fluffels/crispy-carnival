#include "Camera.h"

#include "glm/glm/gtc/matrix_transform.hpp"

using glm::cross;
using glm::lookAt;
using glm::normalize;
using glm::perspective;
using glm::rotate;
using glm::translate;
using glm::vec4;

#define PI 3.14159265358979323846f

#include "easylogging++.h"

#include "MathLib.cpp"

Camera::Camera() {
    rotation = {1, 0, 0, 0};
}

MVP Camera::get() {
    MVP mvp;
    at = location + vec3(0, 5, 0);
    vec3 direction = vec4(1, 0, 0, 0);
    vec3 offset = { direction.x * 15, direction.y * 15, direction.z * 15 };
    eye = at - offset;
    mvp.view = lookAt(eye, at, up);
    mvp.rot = lookAt(vec3(0, 0, 0), normalize(at - eye), up);
    mvp.proj = perspective(fov, ar, nearz, farz);
    // mvp.model = mat4(1);
    // mvp.model = translate(mvp.model, location);

    {
        // float rr[16] = {};
        // quaternionNormalizedToMatrix(rotation, rr);
        // matrixCopy(rr, mvp.model);
        LOG(INFO) << quaternionMagnitude(rotation) << " " << rotation.w << " " << rotation.x << " " << rotation.y << " " << rotation.z;
        mvp.q.x = rotation.x;
        mvp.q.y = rotation.y;
        mvp.q.z = rotation.z;
        mvp.q.w = rotation.w;
        // float translation[16] = {};
        // matrixInit(translation);
        // matrixTranslate(location.x, location.y, location.z, translation);
        // matrixMultiply(translation, rr, mvp.model);
    }

    return mvp;
}

void Camera::tick(float delta) {
    location += velocity * delta;
}

void Camera::setAR(uint32_t w, uint32_t h) {
    ar = (float)w / (float)h;
}

void Camera::setFOV(float f) {
    fov = glm::radians(f);
}

void Camera::back(float d) {
    forward(-d);
}

void Camera::left(float d) {
    right(-d);
}

void Camera::forward(float d) {
    velocity += direction * d;
}

void Camera::right(float d) {
    auto right = normalize(cross(direction, up));
    velocity += right * d;
}

void Camera::rotateY(float d) {
    auto delta = quaternionRotate(0, -1, 0, PI * d * (1/180.f));
    rotation = quaternionMultiply(delta, rotation);
}

void Camera::rotateX(float d) {
    auto delta = quaternionRotate(0, 0, 1, PI * d * (1/180.f));
    rotation = quaternionMultiply(delta, rotation);
}
