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

flat in vec3 v_T;
flat in vec3 v_B;
in vec3 v_N;

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

struct CustomMaterial {
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 emissiveColor;

    float metallic;
    float roughness;
    float shininess;
    float reflectance;
};

layout(std430, binding = 5) buffer CustomMaterials {
    CustomMaterial customMaterials[];
};

// Sampler mapping structure
struct SamplerMapping {
    unsigned short diffuseIndex;
    unsigned short specularIndex;
    unsigned short parallaxIndex;
    unsigned short normalIndex;
    unsigned short customMaterialIndex;
};

// Sampler mappings buffer
layout(std430, binding = 6) buffer SamplerMap {
    SamplerMapping samplerMappings[];
};

// Uniforms
uniform vec3 u_ViewPos;
uniform float u_ParallaxScale;

#include <components/texture/parallax_displace.frag>

void main()
{    
    SamplerMapping mapping = samplerMappings[v_SamplerIndex];
    CustomMaterial material = customMaterials[mapping.customMaterialIndex];

    g_Position = v_FragPos;

    mat3 TBNInterpolated = mat3(v_T, v_B, v_N);
    mat3 TBN = transpose(TBNInterpolated);
    vec3 tangentViewDir = normalize(TBN * u_ViewPos - TBN * v_FragPos);
    vec2 uv_displaced = parallaxMap(v_UV, tangentViewDir, mapping.parallaxIndex);

    float useDisplaced = step(0.5, float(mapping.parallaxIndex)); 
    vec2 sampler_uv = useDisplaced * uv_displaced + (1.0 - useDisplaced) * v_UV;

    if(sampler_uv.x > 1.0 || sampler_uv.y > 1.0 || sampler_uv.x < 0.0 || sampler_uv.y < 0.0)
        discard;

    // Sample normal map and convert from [0,1] to [-1,1]
    vec3 normalMap = texture(normalSamplers[mapping.normalIndex], sampler_uv).rgb;
    normalMap = normalMap * 2.0 - 1.0;
    normalMap.x = -normalMap.x;
    
    // Transform tangent-space normal to world space
    vec3 normal = normalize(TBNInterpolated * normalMap);
    g_Normal = normal;

    // Check if material index is not 0 (1.0 if true, 0.0 if false)
    float hasMaterial = step(0.5, float(mapping.customMaterialIndex));

    // Check if diffuse and specular sampler indices are not 0
    float hasDiffuseSampler = step(0.5, float(mapping.diffuseIndex));
    float hasSpecularSampler = step(0.5, float(mapping.specularIndex));

    // Use sampler only when both material exists AND sampler index exists
    float useDiffuseSampler = hasMaterial * hasDiffuseSampler;
    float useSpecularSampler = hasMaterial * hasSpecularSampler;

    // Sample textures
    vec3 sampledDiffuse = texture(diffuseSamplers[mapping.diffuseIndex], sampler_uv).rgb;
    vec3 sampledSpecular = texture(specularSamplers[mapping.specularIndex], sampler_uv).rgb;

    // Mix between material color and sampler color based on conditions
    g_Albedo = mix(material.diffuseColor, sampledDiffuse, useDiffuseSampler);
    g_Specular = mix(material.specularColor, sampledSpecular, useSpecularSampler);

    // Calculate shininess - mix between material shininess and sampler-based shininess
    float sampledShininess = mix(2.0, 256.0, texture(specularSamplers[mapping.specularIndex], sampler_uv).r);
    float finalShininess = mix(material.shininess, sampledShininess, useSpecularSampler);

    g_Shine_Reflectance = vec2(finalShininess, material.reflectance);
}