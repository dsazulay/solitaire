#version 410

in vec2 v_uv;

uniform sampler2D u_main_tex;
uniform vec3 u_tint;

out vec4 frag_color;

void main()
{
    vec4 tex = texture(u_main_tex, v_uv);
    tex.rgb *= u_tint;
    frag_color = tex;
}
