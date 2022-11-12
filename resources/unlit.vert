#version 410

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;
out float v_instanceID;

uniform mat4 u_proj;
uniform mat4 u_model[60];
uniform vec2 u_offset[60];

void main()
{
    v_instanceID = gl_InstanceID;
    v_uv = a_uv * vec2(0.07692, 0.2) + vec2(0.0765, 0.198) * u_offset[gl_InstanceID] + vec2(0.0025, 0.005);
    gl_Position = u_proj * u_model[gl_InstanceID] * vec4(a_pos, 0.0, 1.0);
}
