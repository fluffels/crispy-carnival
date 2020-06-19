#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=1) uniform sampler2D tex;

layout(location=0) in vec2 inUV;
layout(location=1) in vec3 inNormal;

layout(location=0) out vec4 outColor;

const vec3 lightD = normalize(vec3(1, 1, 1));

void main() {
    vec3 color = texture(tex, inUV).rgb;
    float light = max(0, dot(inNormal, lightD));
    light = clamp(light + .01f, 0, 1);
    outColor = vec4(color * light, 1);
}
