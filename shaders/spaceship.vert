#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "uniforms.glsl"

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inUV;
layout(location=2) in vec3 inNormal;

layout(location=0) out vec2 outUV;
layout(location=1) out vec3 outNormal;

void main() {
    gl_Position = uniforms.proj * uniforms.spaceShipView * vec4(inPosition, 1);
    outUV = inUV;
    outNormal = inNormal;
}
