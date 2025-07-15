//shader vertex
#version 430 core

out vec3 v_WorldPos;
flat out float v_GridSize;
flat out vec3 v_CameraWorldPos;

uniform mat4 u_ViewProjection = mat4(1.0);
uniform vec3 u_CameraWorldPos;
float u_GridSize = 100.0;

const vec3 Pos[4] = vec3[4](
vec3(-1.0, 0.0, -1.0),      // bottom left
vec3( 1.0, 0.0, -1.0),      // bottom right
vec3( 1.0, 0.0,  1.0),      // top right
vec3(-1.0, 0.0,  1.0)       // top left
);

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

void main()
{
    int Index = Indices[gl_VertexID];
    vec3 vPos3 = Pos[Index];
    vPos3 *= u_GridSize;

    vPos3.x += u_CameraWorldPos.x;
    vPos3.z += u_CameraWorldPos.z;

    vec4 vPos4 = vec4(vPos3, 1.0);

    gl_Position = u_ViewProjection * vPos4;

    v_WorldPos = vPos3;
    v_CameraWorldPos = u_CameraWorldPos;
    v_GridSize = u_GridSize;
}