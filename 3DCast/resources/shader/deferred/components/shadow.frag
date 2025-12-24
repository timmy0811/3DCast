//include component

// structure
uniform bool u_HasShadowMap;
uniform sampler2D u_ShadowMap[3];
uniform mat4 u_LightViewProj[3];
uniform float u_CascadeSplits[3]; // linear view-space distances of cascade ends
uniform mat4 u_View;
uniform vec3 u_ShadowLightDir; // Directional light.direction of the caster (same space as directionalLights[].direction)
uniform float u_ShadowBiasFactors[3];

float ComputeShadowCSM(vec3 worldPos, vec3 normal)
{
    if (!u_HasShadowMap) return 1.0;

    // Compute linear view-space depth for cascade selection
    float viewZ = -(u_View * vec4(worldPos, 1.0)).z; // positive forward

    int cascade = 0;
    // u_CascadeSplits holds the far distance of each cascade
    if (viewZ > u_CascadeSplits[0]) cascade = 1;
    if (viewZ > u_CascadeSplits[1]) cascade = 2;

    vec4 lightClip = u_LightViewProj[cascade] * vec4(worldPos, 1.0);
    vec3 ndc = lightClip.xyz / lightClip.w;
    vec3 uvw = ndc * 0.5 + 0.5;

    // Outside light frustum
    if (uvw.x < 0.0 || uvw.x > 1.0 || uvw.y < 0.0 || uvw.y > 1.0)
    return 1.0;

    float bias = max(0.005 * (1.0 - max(dot(normalize(normal), normalize(-u_ShadowLightDir)), 0.0)), 0.0005);
    bias *= u_ShadowBiasFactors[cascade];

    // Filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap[cascade], 0));

    vec2 poissonDisk[16] = vec2[](
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870),
        vec2(0.34495938, 0.29387760),
        vec2(-0.91588581, 0.45771432),
        vec2(-0.81544232, -0.87912464),
        vec2(-0.38277543, 0.27676845),
        vec2(0.97484398, 0.75648379),
        vec2(0.44323325, -0.97511554),
        vec2(0.53742981, -0.47373420),
        vec2(-0.26496911, -0.41893023),
        vec2(0.79197514, 0.19090188),
        vec2(-0.24188840, 0.99706507),
        vec2(-0.81409955, 0.91437590),
        vec2(0.19984126, 0.78641367),
        vec2(0.14383161, -0.14100790)
    );

    // Dithering
    float angle = fract(sin(dot(worldPos.xyz, vec3(12.9898, 78.233, 45.164))) * 43758.5453) * 6.283185307179586;
    float s = sin(angle);
    float c = cos(angle);
    mat2 rot = mat2(c, s, -s, c);

    float radius = 1.5;
    if (cascade == 1) radius = 2.0;
    if (cascade == 2) radius = 3.5;

    for (int i = 0; i < 16; i++)
    {
        vec2 offset = (rot * poissonDisk[i]) * texelSize * radius;
        float pcfDepth = texture(u_ShadowMap[cascade], uvw.xy + offset).r;
        shadow += uvw.z - bias <= pcfDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
}