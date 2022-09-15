#version 410

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;

uniform mat4 u_proj;
uniform mat4 u_model;

void main()
{
    v_uv = a_uv;
    gl_Position = u_proj * u_model * vec4(a_pos, 0.0, 1.0);
}
