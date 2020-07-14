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

Camera::Camera(): rotation(1) {
    qRotation = {1, 0, 0, 0};
}

MVP Camera::get() {
    MVP mvp;
    at = location + vec3(0, 5, 0);
    vec3 direction = rotation * vec4(1, 0, 0, 0);
    vec3 offset = { direction.x * 15, direction.y * 15, direction.z * 15 };
    eye = at - offset;
    mvp.view = lookAt(eye, at, up);
    mvp.rot = lookAt(vec3(0, 0, 0), normalize(at - eye), up);
    mvp.proj = perspective(fov, ar, nearz, farz);
    // mvp.model = mat4(1);
    // mvp.model = translate(mvp.model, location);

    {
        // float rr[16] = {0};
        // quaternionToMatrix(qRotation, rr);
        matrixInit(mvp.model);
        matrixTranslate(location.x, location.y, location.z, mvp.model);
        // mvp.model = translate(mvp.model, location) * rotation;
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
    auto rotation = quaternionRotate(0, -1, 0, PI * d * (1/180.f));
    quaternionNormalize(rotation);
    qRotation = quaternionMultiply(rotation, qRotation);
}

void Camera::rotateX(float d) {
    auto rotation = quaternionRotate(1, 0, 0, PI * d * (1/180.f));
    quaternionNormalize(rotation);
    qRotation = quaternionMultiply(rotation, qRotation);
}
