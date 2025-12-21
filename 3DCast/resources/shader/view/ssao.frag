//shader fragment
#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gBuf_Position;
uniform sampler2D gBuf_Normal;

uniform sampler2D texNoise;
uniform vec3 ssaoSamples[64];

uniform vec2 screenSize = vec2(800.0, 600.0);
uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.025;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec2 noiseScale = vec2(screenSize.x / 4.0, screenSize.y / 4.0);

    vec3 fragPosWorld = texture(gBuf_Position, TexCoords).xyz;
    vec3 normalWorld = normalize(texture(gBuf_Normal, TexCoords).rgb);

    // Early out non geometry
    if (length(fragPosWorld) < 1e-6 || length(normalWorld) < 1e-6) {
        FragColor = 1.0;
        return;
    }

    // Transform to view space
    vec3 fragPos = (view * vec4(fragPosWorld, 1.0)).xyz;
    vec3 normal = normalize(mat3(view) * normalWorld);

    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    // Create TBN from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // Calc occlusion factor
    float occlusion = 0.0;
    float validSamples = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // Transform to view space
        vec3 samplePos = TBN * vec3(ssaoSamples[i]);
        samplePos = fragPos + samplePos * radius;

        // project sample position
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // map to range 0.0 - 1.0

        // skip if projected out of screen
        if (offset.x < 0.0 || offset.y < 0.0 || offset.x > 1.0 || offset.y > 1.0)
            continue;

        // get sample depth: transform sampled world position to view space and take z
        vec3 sampleWorld = texture(gBuf_Position, offset.xy).xyz;
        if (length(sampleWorld) < 1e-6)
            continue; // background texel
        float sampleDepth = (view * vec4(sampleWorld, 1.0)).z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
        validSamples += 1.0;
    }

    occlusion = (validSamples > 0.0) ? 1.0 - (occlusion / validSamples) : 1.0;

    FragColor = occlusion;
}