//shader fragment
#version 430 core

layout(location = 0) out vec4 o_Color;

in vec2 v_UV;

uniform sampler2D u_Pallete;

void main() {
    vec4 texColor = texture(u_Pallete, v_UV);
    
    if (texColor.a < 0.1)
        discard;

    o_Color = texColor;
}