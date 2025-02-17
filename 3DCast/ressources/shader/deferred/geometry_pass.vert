//shader vertex
#version 430 core

layout(location = 0) in vec3 a_Position; // +
layout(location = 1) in vec3 a_Normal; // +
layout(location = 2) in vec2 a_UV;  // +
//layout(location = 3) in float a_TexIndex; // +
layout(location = 3) in float a_TransformIndex; // +
//layout(location = 5) in vec2 a_Shine_Reflectance;

out vec3 v_FragPos;
out vec2 v_UV;
flat out vec3 v_Normal;
flat out int v_TexIndex;
flat out vec2 v_Shine_Reflectance;

uniform mat4 u_View;
uniform mat4 u_Projection;

layout(std430, binding = 0) buffer TransformMatrices {
    mat4 transformMatrices[];
};

void main()
{
    mat4 model = transformMatrices[int(a_TransformIndex)];
    vec4 position = model * vec4(a_Position, 1.0);
    v_FragPos = position.xyz;
    v_UV = a_UV;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    v_Normal = normalMatrix * a_Normal;

    v_TexIndex = int(0);
    //v_Shine_Reflectance = a_Shine_Reflectance;
    v_Shine_Reflectance = vec2(32.0, 0.5);
    gl_Position = u_Projection * u_View * position;
};