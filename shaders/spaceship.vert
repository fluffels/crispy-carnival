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

// See: https://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec4 quat_conj(vec4 q)
{ 
  return vec4(-q.x, -q.y, -q.z, q.w); 
}
  
vec4 quat_mult(vec4 q1, vec4 q2)
{ 
  vec4 qr;
  qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
  qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
  qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
  qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
  return qr;
}

vec3 rotate_vertex_position(vec3 position)
{ 
  // vec4 qr = quat_from_axis_angle(axis, angle);
  vec4 qr = uniforms.quat;
  vec4 qr_conj = quat_conj(qr);
  vec4 q_pos = vec4(position.x, position.y, position.z, 0);
  
  vec4 q_tmp = quat_mult(qr, q_pos);
  qr = quat_mult(q_tmp, qr_conj);
  
  return vec3(qr.x, qr.y, qr.z);
}

void main() {
    vec3 P = rotate_vertex_position(inPosition);
    gl_Position = uniforms.proj * uniforms.view * vec4(P, 1.0);
    outUV = inUV;
    outNormal = inNormal;
}
