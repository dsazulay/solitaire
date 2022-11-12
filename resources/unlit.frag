#version 410

in vec2 v_uv;
in float v_instanceID;

uniform sampler2D u_main_tex;
uniform vec3 u_tint[60];

out vec4 frag_color;

void main()
{
    vec4 tex = texture(u_main_tex, v_uv);
    tex.rgb *= u_tint[int(v_instanceID)];
    frag_color = tex;
}
