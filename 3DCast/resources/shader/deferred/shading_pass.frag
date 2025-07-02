//shader fragment
#version 430 core

layout(location = 0) out vec4 o_Color;

// GBuffer
uniform sampler2D gBuf_Position;
uniform sampler2D gBuf_Normal;
uniform sampler2D gBuf_Albedo;
uniform sampler2D gBuf_Specular;
uniform sampler2D gBuf_Shine_Reflectance;

uniform sampler2D gBuf_Depth;

// Uniforms
uniform vec2 u_Resolution;
uniform vec3 u_ViewPosition;

uniform int BufferCountPointLight;
uniform int BufferCountDirectionalLight;
uniform int BufferCountSpotLight;

// components
#include <components/light/directional_shade.frag>
#include <components/light/point_shade.frag>
#include <components/light/spot_shade.frag>

// dynamic buffers
layout(std430, binding = 1) buffer DirectionalLightBuffer {
    DirectionalLight directionalLights[]; 
};

layout(std430, binding = 2) buffer SpotLightBuffer {
    SpotLight spotLights[]; 
};

layout(std430, binding = 3) buffer PointLightBuffer {
    PointLight pointLights[]; 
};

void main()
{    
	vec2 fragCoord = gl_FragCoord.xy / u_Resolution;
    vec3 fragPos = texture(gBuf_Position, fragCoord).xyz;

	vec3 color = vec3(0.0);
    vec3 viewDirection = normalize(u_ViewPosition - fragPos);
    vec3 normal = texture(gBuf_Normal, fragCoord).rgb;

    float shine = texture(gBuf_Shine_Reflectance, fragCoord).r;
    vec3 albedo = texture(gBuf_Albedo, fragCoord).rgb;
    vec3 specular = texture(gBuf_Specular, fragCoord).rgb;

    // Directional Light
    for(int i = 0; i < BufferCountDirectionalLight; i++){
        color += AffectDirectionallight(directionalLights[i], normal, viewDirection, shine, albedo, specular);
    }

    // Point Lights
    for(int i = 0; i < BufferCountPointLight; i++){
        color += AffectPointlight(pointLights[i], normal, viewDirection, fragPos, shine, albedo, specular);
    }

    // Spotlight
    for(int i = 0; i < BufferCountSpotLight; i++){
        color += AffectSpotlight(spotLights[i], normal, fragPos, viewDirection, albedo, shine, specular);
    }

    o_Color = vec4(color, 1.0);
}
