//shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_UV;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main(){
    o_Color = texture(u_Texture, v_UV);
    o_Color = vec4(v_UV.y, 0.0, 0.0, 1.0);
}