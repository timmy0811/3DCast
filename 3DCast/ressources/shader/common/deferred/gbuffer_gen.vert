//shader vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in int a_TexIndex;
layout(location = 4) in int a_TransformIndex;
layout(location = 5) in vec2 a_Shine_Reflectance;

out vec3 v_FragPos;
out vec2 v_UV;
out vec3 v_Normal;
out int v_TexIndex;
out vec2 v_Shine_Reflectance;

uniform mat4 u_View;
uniform mat4 u_Projection;

layout(std430, binding = 0) buffer TransformMatrices {
    mat4 transformMatrices[];
};

void main()
{
    vec4 viewPos = u_View * vec4(a_Position, 1.0);
    v_FragPos = viewPos.xyz;
    v_UV = a_UV;
    v_Normal = normalize(a_Normal);
    v_TexIndex = a_TexIndex;
    v_Shine_Reflectance = a_Shine_Reflectance;
    gl_Position = u_Projection * viewPos;
};