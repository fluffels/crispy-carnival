#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 inColor;
layout(location=1) in vec3 inNormal;

layout(location=0) out vec4 outColor;

const vec3 lightD = normalize(vec3(1, 1, 1));

void main() {
    float light = max(0, dot(inNormal, lightD));
    light = clamp(light + .01f, 0, 1);
    outColor = vec4(inColor * light, 1);
}
