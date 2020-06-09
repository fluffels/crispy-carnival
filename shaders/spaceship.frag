#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=1) uniform sampler2D tex;

layout(location=0) out vec4 outColor;
layout(location=1) out vec2 inUV;

void main() {
    outColor = vec4(0);
}
