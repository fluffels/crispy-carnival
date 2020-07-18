#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=0) uniform Uniform {
    mat4x4 proj;
    mat4x4 view;
    mat4x4 model;
    mat4x4 rot;
    vec4 quat;
} uniforms;

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inUV;
layout(location=2) in vec3 inNormal;

layout(location=0) out vec2 outUV;
layout(location=1) out vec3 outNormal;

void main() {
    gl_Position = uniforms.proj * uniforms.view * vec4(inPosition, 1);
    outUV = inUV;
    outNormal = inNormal;
}
