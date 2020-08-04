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
    location = {0, 0, 0};
    stop();
}

MVP Camera::get() {
    MVP mvp;
    mvp.proj = perspective(fov, ar, nearz, farz);

    matrixInit(mvp.spaceShipView);
    matrixTranslate(0, 1, -5, mvp.spaceShipView);

    matrixInit(mvp.planetModelView);
    matrixScale(100, mvp.planetModelView);
    matrixTranslate(0, 0, -500, mvp.planetModelView);
    matrixTranslate(-location.x, -location.y, -location.z, mvp.planetModelView);

    mvp.skyboxRotation.x = rotation.x;
    mvp.skyboxRotation.y = rotation.y;
    mvp.skyboxRotation.z = rotation.z;
    mvp.skyboxRotation.w = rotation.w;

    return mvp;
}

void Camera::getDebugString(char* buffer) {
    Quaternion dir = {};
    dir.z = 1;
    quaternionUnrotate(rotation, dir);
    sprintf_s(buffer, 256, "(%+f %+f %+f) |%+f|", -dir.x, -dir.y, -dir.z, quaternionMagnitude(dir));
}

void Camera::tick(float delta) {
    location += velocity * delta;
    rotation = quaternionMultiply(angularMomentum, rotation);
    quaternionNormalize(rotation);
}

void Camera::stop() {
    angularMomentum = {0, 0, 0, 1};
    velocity = {};
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
    Quaternion dir = {};
    dir.z = 1;
    quaternionUnrotate(rotation, dir);
    velocity.x -= d * dir.x;
    velocity.y -= d * dir.y;
    velocity.z -= d * dir.z;
}

void Camera::right(float d) {
    Quaternion dir = {};
    dir.x = 1;
    quaternionUnrotate(rotation, dir);
    velocity.x += d * dir.x;
    velocity.y += d * dir.y;
    velocity.z += d * dir.z;
}

void Camera::rotateY(float d) {
    auto delta = quaternionFromAngleAxis(0, 1, 0, PI * d * (1/180.f));
    angularMomentum = quaternionMultiply(delta, angularMomentum);
    quaternionNormalize(angularMomentum);
}

void Camera::rotateX(float d) {
    auto delta = quaternionFromAngleAxis(1, 0, 0, PI * d * (1/180.f));
    angularMomentum = quaternionMultiply(delta, angularMomentum);
    quaternionNormalize(angularMomentum);
}

void Camera::rotateZ(float d) {
    auto delta = quaternionFromAngleAxis(0, 0, 1, PI * d * (1/180.f));
    angularMomentum = quaternionMultiply(delta, angularMomentum);
    quaternionNormalize(angularMomentum);
}
