//shader fragment
#version 430 core

layout (location = 0) out vec3 g_Position;
layout (location = 1) out vec3 g_Normal;
layout (location = 2) out vec3 g_Albedo;
layout (location = 3) out vec3 g_Specular;
layout (location = 4) out vec2 g_Shine_Reflectance;

in vec3 v_FragPos;
in vec2 v_UV;
flat in int v_SamplerIndex;
flat in mat3 v_TBN;

layout(std430, binding = 1) buffer DiffuseSamplers {
    sampler2D diffuseSamplers; // diffuseSamplers[0] is neutral Element
};

layout(std430, binding = 2) buffer SpecularSamplers {
    sampler2D specularSamplers; // specularSamplers[0] is neutral Element
};

layout(std430, binding = 3) buffer ShininessSamplers {
    sampler2D shininessSamplers; // shininessSamplers[0] is neutral Element
};

layout(std430, binding = 4) buffer NormalSamplers{
    sampler2D normalSamplers; // normalSamplers[0] is neutral Element
};

struct SamplerMapping{
    unsigned short diffuseIndex;
    unsigned short specularIndex;
    unsigned short shininessIndex;
};

layout(std430, binding = 4) buffer SamplerMap {
    SamplerMapping samplerMap;
};

void main()
{    
    SamplerMapping mapping = samplerMap[v_SamplerIndex];

    g_Position = v_FragPos;
    g_Normal = v_Normal;

    g_Albedo = diffuseSamplers[mapping.diffuseIndex].xyz;
    g_Specular = specularSamplers[mapping.specularIndex].xyz;

    g_Shine_Reflectance = vec2(shininessSamplers[mapping.shininessIndex], 0.0);
}