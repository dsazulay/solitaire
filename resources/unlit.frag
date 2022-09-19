#version 410

in vec2 v_uv;

uniform sampler2D u_main_tex;

out vec4 frag_color;

void main()
{
    frag_color = texture(u_main_tex, v_uv);
}
