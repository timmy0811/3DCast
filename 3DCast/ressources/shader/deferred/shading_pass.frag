//shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

// GBuffer
uniform sampler2D gBuf_Position;
uniform sampler2D gBuf_Normal;
uniform sampler2D gBuf_Albedo;
uniform sampler2D gBuf_Specular;
uniform sampler2D gBuf_Shine_Reflectance;

uniform sampler2D gBuf_Depth;

uniform vec2 u_Resolution;

void main()
{    
	vec2 pixelPos = gl_FragCoord.xy / u_Resolution;
    o_Color = vec4(texture(gBuf_Normal, pixelPos).xyz, 1.0);
}