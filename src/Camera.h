#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using glm::mat4;
using glm::vec3;

struct MVP {
    mat4 view;
    mat4 proj;
};

struct Camera {
    vec3 eye;
    vec3 at;
    vec3 up;

    float fov;
    float ar;

    float nearz;
    float farz;

    MVP get() const;

    void setAR(uint32_t, uint32_t);
    void setFOV(float);

    void back(float);
    void forward(float);
    void left(float);
    void right(float);
    
    void rotateY(float);
    void rotateX(float);
};
