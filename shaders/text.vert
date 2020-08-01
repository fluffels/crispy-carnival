#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location=0) in vec3 position;
layout (location=1) in vec4 inColor;

layout (location=0) out vec4 outColor;

void main() {
    gl_Position = vec4(position.x / 100.f, position.y / 100.f, 0.000001, 1);
    outColor = inColor;
}
