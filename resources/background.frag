#version 410

in vec2 v_uv;

uniform vec3 u_tint;

out vec4 frag_color;

void main()
{
    frag_color = vec4(u_tint, 1.0);
}
