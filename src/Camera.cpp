#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

using glm::cross;
using glm::lookAt;
using glm::normalize;
using glm::perspective;
using glm::rotate;
using glm::translate;
using glm::vec4;

#define PI 3.14159265358979323846f

Camera::Camera(): rotation(1) {}

MVP Camera::get() {
    MVP mvp;
    at = location + vec3(0, 5, 0);
    eye = at - vec3(15, -5, 0);
    mvp.view = lookAt(eye, at, up);
    mvp.rot = lookAt(vec3(0, 0, 0), normalize(at - eye), up);
    mvp.proj = perspective(fov, ar, nearz, farz);
    mvp.model = mat4(1);
    mvp.model = translate(mvp.model, location) * rotation;
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
    rotation = rotate(rotation, PI * d * (1/180.f), up);
}

void Camera::rotateX(float d) {
    vec3 f = at - eye;
    vec4 forward = vec4(f, 0.0);
    auto right = normalize(cross(f, up));
    rotation = rotate(rotation, PI * d * (1/180.f), right);
}
