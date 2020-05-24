#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=1) uniform samplerCube atlas;

layout(location=0) in vec4 inViewDir;
layout(location=1) in flat uint inTexIdx;

layout(location=0) out vec4 outColor;

void main() {
    outColor = texture(atlas, inViewDir.xyz);
}
