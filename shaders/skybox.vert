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

layout(location=0) out vec4 outViewDir;

void main() {
    gl_Position = uniforms.proj * uniforms.rot * vec4(inPosition, 1.0);
    outViewDir = vec4(inPosition, 0);
}
