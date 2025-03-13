//shader fragment
#version 430 core
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler) uniform sampler2D bindless;

layout (location = 0) out vec3 g_Position;
layout (location = 1) out vec3 g_Normal;
layout (location = 2) out vec3 g_Albedo;
layout (location = 3) out vec3 g_Specular;
layout (location = 4) out vec2 g_Shine_Reflectance;

in vec3 v_FragPos;
in vec2 v_UV;
flat in int v_SamplerIndex;
flat in mat3 v_TBN;

// Bindless texture samplers
layout(std430, binding = 1) buffer DiffuseSamplers {
    sampler2D diffuseSamplers[]; // diffuseSamplers[0] is neutral Element
};

layout(std430, binding = 2) buffer SpecularSamplers {
    sampler2D specularSamplers[]; // specularSamplers[0] is neutral Element
};

layout(std430, binding = 3) buffer ParallaxSamplers {
    sampler2D parallaxSamplers[]; // parallaxSamplers[0] is neutral Element
};

layout(std430, binding = 4) buffer NormalSamplers {
    sampler2D normalSamplers[]; // normalSamplers[0] is neutral Element
};

// Sampler mapping structure
struct SamplerMapping {
    unsigned short diffuseIndex;
    unsigned short specularIndex;
    unsigned short parallaxIndex;
    unsigned short normalIndex;
};

// Sampler mappings buffer
layout(std430, binding = 5) buffer SamplerMap {
    SamplerMapping samplerMappings[];
};

// Uniforms
uniform vec3 u_ViewPos;
uniform float u_ParallaxScale;

#define PARALLAX_SCALE 0.03
#include <components/texture/parallax_displace.frag>

void main()
{    
    SamplerMapping mapping = samplerMappings[v_SamplerIndex];

    g_Position = v_FragPos;

    mat3 TBN = transpose(v_TBN);
    vec3 tangentViewDir = normalize(TBN * u_ViewPos - TBN * v_FragPos);
    vec2 uv_displaced = parallaxMap(v_UV, tangentViewDir, mapping.parallaxIndex);

    float useDisplaced = step(0.5, float(mapping.parallaxIndex)); 
    vec2 sampler_uv = useDisplaced * uv_displaced + (1.0 - useDisplaced) * v_UV;

    if(sampler_uv.x > 1.0 || sampler_uv.y > 1.0 || sampler_uv.x < 0.0 || sampler_uv.y < 0.0)
        discard;

    // Sample the normal map and convert from [0,1] to [-1,1]
    vec3 normalMap = texture(normalSamplers[mapping.normalIndex], sampler_uv).rgb;
    normalMap = normalMap * 2.0 - 1.0;
    normalMap.x = -normalMap.x;
    
    // Transform the tangent-space normal to world space
    vec3 normal = normalize(v_TBN * normalMap);
    g_Normal = normal;

    g_Albedo = texture(diffuseSamplers[mapping.diffuseIndex], sampler_uv).rgb;
    g_Specular = texture(specularSamplers[mapping.specularIndex], sampler_uv).rgb;
    float shininess = mix(4.0, 32.0, texture(specularSamplers[mapping.specularIndex], sampler_uv).r); // Using first bit of specular map for shininess

    g_Shine_Reflectance = vec2(shininess, 0.5);
}