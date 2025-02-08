//shader fragment
#version 430 core

layout (location = 0) out vec3 g_Position;
layout (location = 1) out vec3 g_Normal;
layout (location = 2) out vec3 g_Albedo;
layout (location = 3) out vec3 g_Specular;
layout (location = 4) out vec2 g_Shine_Reflectance;

in vec3 v_FragPos;
in vec2 v_UV;
flat in vec3 v_Normal;
flat in int v_TexIndex;
flat in vec2 v_Shine_Reflectance;

void main()
{    
    g_Position = v_FragPos;
    g_Normal = v_Normal;

    // Determine by Texture Sample
    g_Albedo = vec3(1.0, 0.1, 0.8);
    g_Specular = vec3(1.0);

    g_Shine_Reflectance = v_Shine_Reflectance;
}