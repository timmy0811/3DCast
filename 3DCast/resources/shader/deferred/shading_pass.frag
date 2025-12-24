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

// SSAO
uniform sampler2D u_SSAO;
uniform float u_SSAOAffectness;

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

#include <components/shadow.frag>

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

    vec2 shine_reflectance = texture(gBuf_Shine_Reflectance, fragCoord).rg;
    float shine = shine_reflectance.r;
    float reflectance = shine_reflectance.g;

    vec3 albedo = texture(gBuf_Albedo, fragCoord).rgb;
    vec3 specular = texture(gBuf_Specular, fragCoord).rgb * reflectance;

    // Directional Lights (apply shadow only to the active caster)
    int casterIndex = -1;
    if (u_HasShadowMap)
    {
        float bestDot = -1.0;
        for (int i = 0; i < BufferCountDirectionalLight; ++i)
        {
            // Match light by direction (same value as on CPU)
            float d = dot(normalize(directionalLights[i].direction), normalize(u_ShadowLightDir));
            if (d > bestDot)
            {
                bestDot = d;
                casterIndex = i;
            }
        }
    }

    for (int i = 0; i < BufferCountDirectionalLight; i++)
    {
        vec3 unshadowed = AffectDirectionallight(directionalLights[i], normal, viewDirection, shine, albedo, specular);
        if (u_HasShadowMap && i == casterIndex)
        {
            vec3 ambientOnly = directionalLights[i].ambient * albedo;
            float vis = ComputeShadowCSM(fragPos, normal);
            // Preserve ambient, shadow diffuse+specular
            color += ambientOnly + vis * (unshadowed - ambientOnly);
        }
        else
        {
            color += unshadowed;
        }
    }

    // Point Lights
    for(int i = 0; i < BufferCountPointLight; i++){
        color += AffectPointlight(pointLights[i], normal, viewDirection, fragPos, shine, albedo, specular);
    }

    // Spotlight
    for(int i = 0; i < BufferCountSpotLight; i++){
        color += AffectSpotlight(spotLights[i], normal, fragPos, viewDirection, albedo, shine, specular);
    }

    float ssao = texture(u_SSAO, fragCoord).r;
    float ssaoMix = mix(1.0, ssao, clamp(u_SSAOAffectness, 0.0, 1.0));
    o_Color = vec4(color * ssaoMix, 1.0);
}
