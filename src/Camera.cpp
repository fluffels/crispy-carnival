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
    rotation = {0, 0, 0, 1};
    angularMomentum = {0, 0, 0, 1};
}

MVP Camera::get() {
    MVP mvp;
    mvp.proj = perspective(fov, ar, nearz, farz);
    at = location + vec3(0, 5, 0);
    vec3 direction = vec4(1, 0, 0, 0);
    vec3 offset = { direction.x * 15, direction.y * 15, direction.z * 15 };
    eye = at - offset;
    mvp.spaceShipModelView = lookAt(eye, at, up);

    {
        LOG(INFO) << quaternionMagnitude(rotation) << " " << rotation.w << " " << rotation.x << " " << rotation.y << " " << rotation.z;
        mvp.skyboxRotation.x = rotation.x;
        mvp.skyboxRotation.y = rotation.y;
        mvp.skyboxRotation.z = rotation.z;
        mvp.skyboxRotation.w = rotation.w;
    }

    return mvp;
}

void Camera::tick(float delta) {
    location += velocity * delta;
    rotation = quaternionMultiply(angularMomentum, rotation);
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
    auto delta = quaternionRotate(0, 1, 0, PI * d * (1/180.f));
    angularMomentum = quaternionMultiply(delta, angularMomentum);
}

void Camera::rotateX(float d) {
    auto delta = quaternionRotate(1, 0, 0, PI * d * (1/180.f));
    angularMomentum = quaternionMultiply(delta, angularMomentum);
}
