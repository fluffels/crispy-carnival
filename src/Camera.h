#pragma once

#include "glm/glm/mat4x4.hpp"
#include "glm/glm/vec3.hpp"

#include "MathLib.h"

using glm::mat4;
using glm::vec3;

struct MVP {
    mat4 proj;
    mat4 spaceShipModelView;
    float planetModelView[16];
    Quaternion skyboxRotation;
};

struct Camera {
    vec3 eye;
    vec3 at;
    vec3 up;

    vec3 direction;
    vec3 location;
    vec3 velocity;

    Quaternion rotation;
    Quaternion angularMomentum;

    Camera();

    float fov;
    float ar;

    float nearz;
    float farz;

    MVP get();
    void tick(float);

    void setAR(uint32_t, uint32_t);
    void setFOV(float);

    void back(float);
    void forward(float);
    void left(float);
    void right(float);
    
    void rotateY(float);
    void rotateX(float);
    void rotateZ(float);
};
