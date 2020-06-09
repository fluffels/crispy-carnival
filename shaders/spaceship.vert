#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=0) uniform Uniform {
    mat4x4 mvp;
} uniforms;

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inUV;

layout(location=0) out vec2 outUV;

void main() {
    gl_Position = uniforms.mvp * vec4(inPosition, 1.0);
    outUV = inUV;
}
