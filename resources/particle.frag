#version 410

in vec2 v_uv;
in vec4 v_color;

out vec4 frag_color;

void main()
{
    frag_color = v_color;
}
