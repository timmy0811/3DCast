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

flat out vec3 v_T;
flat out vec3 v_B;
out vec3 v_N;

noperspective out vec2 v_AffineUV;
noperspective out float v_AffineW;

uniform mat4 u_View;
uniform mat4 u_Projection;

uniform bool u_VertexSnappingEnabled;
uniform float u_VertexSnappingResolution;

layout(std430, binding = 0) buffer TransformMatrices {
    mat4 transformMatrices[];
};

vec4 SnapToGrid(vec4 clipPos, float resolution)
{
    // Convert to NDC
    vec2 ndc = clipPos.xy / clipPos.w;

    // Snap to grid
    vec2 snapped = floor(ndc * resolution + 0.5) / resolution;

    // Convert back to clip space
    return vec4(snapped * clipPos.w, clipPos.z, clipPos.w);
}

void main()
{
    mat4 model = transformMatrices[int(a_TransformIndex)];
    vec4 position = model * vec4(a_Position, 1.0);
    v_FragPos = position.xyz;
    v_UV = a_UV;

    v_T = normalize(vec3(model * vec4(a_Tangent, 0.0)));
    v_B = normalize(vec3(model * vec4(a_Bitangent, 0.0)));
    v_N = normalize(vec3(model * vec4(a_Normal, 0.0)));

    v_SamplerIndex = int(a_SamplerIndex);

    vec4 clipPos = u_Projection * u_View * position;

    // Vertex snapping
    if (u_VertexSnappingEnabled)
    {
        clipPos = SnapToGrid(clipPos, u_VertexSnappingResolution);
    }

    gl_Position = clipPos;

    // For affine texture mapping: pass UV * W and W with noperspective interpolation
    // In the fragment shader: affineUV = v_AffineUV / v_AffineW gives us linearly interpolated UVs
    v_AffineUV = a_UV * clipPos.w;
    v_AffineW = clipPos.w;
}
