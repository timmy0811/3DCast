//shader vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_UV;
layout(location = 5) in float a_SamplerIndex;
layout(location = 6) in float a_TransformIndex;

out vec3 v_FragPos;
out vec2 v_UV;
flat out int v_SamplerIndex;
flat out mat3 v_TBN;

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

    vec3 T = normalize(vec3(model * vec4(a_Tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(a_Bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(a_Normal, 0.0)));
    v_TBN = mat3(T, B, N);

    v_SamplerIndex = int(a_SamplerIndex);
    gl_Position = u_Projection * u_View * position;
}
