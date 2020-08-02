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

layout(location=0) out vec4 outViewDir;

void main() {
    vec3 P = rotate_vertex_position(uniforms.quat, inPosition);
    gl_Position = uniforms.proj * vec4(P, 1.0);
    outViewDir = vec4(inPosition, 0);
}
