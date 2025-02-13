//shader vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in float a_UVIndex;

uniform vec3 u_CameraPos;
uniform mat4 u_ViewProjection;

out vec2 v_UV;

struct BillboardSource {
	vec3 position;
	float scale;
	int iconId;
};

struct IconData {
	vec4 uvx;
	vec4 uvy;
	int icon;
};

layout(std430, binding = 4) buffer SourceMatrices {
    BillboardSource sources[];
};

layout(std430, binding = 5) buffer Icons {
    IconData icons[];
};

mat4 billboard(vec3 entityPos, vec3 cameraPos) {
    vec3 forward = normalize(cameraPos - entityPos);
    vec3 right = normalize(cross(vec3(0, 1, 0), forward));
    vec3 up = cross(forward, right);

    return mat4(
        vec4(right, 0.0),
        vec4(up, 0.0),
        vec4(forward, 0.0),
        vec4(entityPos, 1.0)
    );
}

float matchValue(float inputVal, float target) {
    return 1.0 - step(0.5, abs(inputVal - target));
}

void main() {
	BillboardSource source = sources[gl_InstanceID];
    mat4 billboardMat = billboard(source.position, u_CameraPos);
    
    float scale = clamp(source.scale, 0.1, 3.0);

    mat4 scaleMat = mat4(
        vec4(scale, 0.0, 0.0, 0.0),
        vec4(0.0, scale, 0.0, 0.0),
        vec4(0.0, 0.0, scale, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    float isCorrectIndex0 = matchValue(0, a_UVIndex);
    float isCorrectIndex1 = matchValue(1, a_UVIndex);
    float isCorrectIndex2 = matchValue(2, a_UVIndex);
    float isCorrectIndex3 = matchValue(3, a_UVIndex);

    v_UV.x = icons[source.iconId].uvx.x * isCorrectIndex0
        + icons[source.iconId].uvx.y * isCorrectIndex1
        + icons[source.iconId].uvx.z * isCorrectIndex2
        + icons[source.iconId].uvx.w * isCorrectIndex3;

    v_UV.y = icons[source.iconId].uvy.x * isCorrectIndex0
        + icons[source.iconId].uvy.y * isCorrectIndex1
        + icons[source.iconId].uvy.z * isCorrectIndex2
        + icons[source.iconId].uvy.w * isCorrectIndex3;

    gl_Position = u_ViewProjection * billboardMat * scaleMat * vec4(a_Position, 1.0);
}