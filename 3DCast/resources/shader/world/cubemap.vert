//shader vertex
#version 330 core

out vec3 UV;
out vec3 v_FragPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

const vec3 Pos[36] = vec3[36](
    vec3(-1.0,  1.0, -1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0,  1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),

    vec3(-1.0, -1.0,  1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3(-1.0, -1.0,  1.0),

    vec3(1.0, -1.0, -1.0),
    vec3(1.0, -1.0,  1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(1.0,  1.0, -1.0),
    vec3(1.0, -1.0, -1.0),

    vec3(-1.0, -1.0,  1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(1.0, -1.0,  1.0),
    vec3(-1.0, -1.0,  1.0),

    vec3(-1.0,  1.0, -1.0),
    vec3(1.0,  1.0, -1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3(-1.0,  1.0, -1.0),

    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0, -1.0,  1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(-1.0, -1.0,  1.0),
    vec3(1.0, -1.0,  1.0)
); 

void main()
{
    vec3 position = Pos[gl_VertexID];
    UV = position;
    v_FragPos = vec3(vec4(position, 1.0));

    vec4 pos = u_Projection * u_View * vec4(position, 1.0);
    gl_Position = pos.xyww;
};