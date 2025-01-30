//shader fragment
#version 430 core

layout (location = 0) out vec3 g_Position;
layout (location = 1) out vec3 g_Normal;
layout (location = 2) out vec3 g_Albedo;
layout (location = 3) out vec3 g_Specular;
layout (location = 4) out vec2 g_Shine_Reflectance;

in vec3 v_FragPos;
in vec2 v_UV;
in vec3 v_Normal;
in int v_TexIndex;
in vec2 v_Shine_Reflectance;

uniform vec3 u_Color;

void main()
{    
    g_Position = v_FragPos;
    g_Normal = v_Normal;

    // Determin by Texture Sample
    g_Albedo = u_Color;
    g_Specular = vec3(1.0);

    g_Shine_Reflectance = v_Shine_Reflectance;
}