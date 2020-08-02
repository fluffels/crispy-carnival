#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "quaternion.glsl"

layout(binding=0) uniform Uniform {
    mat4x4 proj;
    mat4x4 spaceShipModelView;
    mat4x4 planetModelView;
    vec4 quat;
} uniforms;

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inColor;
layout(location=2) in vec3 inNormal;

layout(location=0) out vec3 outColor;
layout(location=1) out vec3 outNormal;

void main() {
    vec3 P = (uniforms.planetModelView * vec4(inPosition, 1.0)).xyz;
    P = rotate_vertex_position(uniforms.quat, P);
    gl_Position = uniforms.proj * vec4(P, 1.0);
    outColor = inColor;
    outNormal = inNormal;
}
