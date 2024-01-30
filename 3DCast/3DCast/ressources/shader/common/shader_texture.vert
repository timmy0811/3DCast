//shader vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_UV;

out vec2 v_UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    v_UV = a_UV;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0 , 1.0);
};