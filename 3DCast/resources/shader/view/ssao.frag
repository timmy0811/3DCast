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
    // tile noise texture over screen based on screen dimensions divided by noise size
    vec2 noiseScale = vec2(screenSize.x / 4.0, screenSize.y / 4.0);

    // get input for SSAO algorithm
    vec3 fragPosWorld = texture(gBuf_Position, TexCoords).xyz;
    vec3 normalWorld = normalize(texture(gBuf_Normal, TexCoords).rgb);

    // transform to view space
    vec3 fragPos = (view * vec4(fragPosWorld, 1.0)).xyz;
    vec3 normal = normalize(mat3(view) * normalWorld);

    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position in view space
        vec3 samplePos = TBN * vec3(ssaoSamples[i]); // from tangent to view-space
        samplePos = fragPos + samplePos * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth: transform sampled world position to view space and take z
        vec3 sampleWorld = texture(gBuf_Position, offset.xy).xyz;
        float sampleDepth = (view * vec4(sampleWorld, 1.0)).z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}