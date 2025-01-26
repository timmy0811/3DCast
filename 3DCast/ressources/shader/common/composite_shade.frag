#shader fragment
#version 330 core
precision mediump float;

layout(location = 0) out vec4 o_Color;

// components
#include <components/light/directional_shade.frag>
#include <components/light/point_shade.frag>
#include <components/light/spot_shade.frag>

// dynamic buffers
layout(std430, binding = 0) buffer PointLightBuffer {
    PointLight pointLights[]; 
};

layout(std430, binding = 1) buffer DirectionalLightBuffer {
    DirectionalLight directionalLights[]; 
};

layout(std430, binding = 2) buffer SpotLightBuffer {
    SpotLight spotLights[]; 
};

// varyings
in vec3 v_FragPos;
in vec3 v_Normal;

// uniforms
uniform int BufferCountPointLight;
uniform int BufferCountDirectionalLight;
uniform int BufferCountSpotLight;

uniform vec3 u_ViewPosition;

// --- Main Shading ---
int main(){
    vec3 output = vec3(0.0);
    vec3 viewDirection = normalize(u_ViewPosition - v_FragPos);
    vec3 norm = normalize(v_Normal);

    // Directional Light
    for(int i = 0; i < BufferCountDirectionalLight; i++){
        output += AffectDirectionallight(directionalLights[i], norm, viewDirection);
    }

    // Point Lights
    for(i = 0; i < BufferCountPointLight; i++){
        output += AffectPointlight(pointLights[i], norm, viewDirection, v_FragPos);
    }

    // Spotlight
    for(i = 0; i < BufferCountSpotLight; i++){
        output += AffectSpotlight(spotLights[i], norm);
    }

    o_Color = vec4(output, 1.0);
}