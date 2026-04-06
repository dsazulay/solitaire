#version 410

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;
out vec4 v_color;

layout (std140) uniform Matrices
{
    uniform mat4 u_proj;
};

uniform mat4 u_model;
uniform vec2 u_offset[300];
uniform vec4 u_color[300];

void main()
{
    v_uv = a_uv;
    v_color = u_color[gl_InstanceID];
    gl_Position = u_proj * u_model * vec4(a_pos.xy + u_offset[gl_InstanceID], 0.0, 1.0);
}
