//include component

// Classic 4x4 Bayer matrix dithering for retro aesthetic

// Dithering uniforms
uniform bool u_DitheringEnabled;
uniform float u_DitheringStrength;     // 0.0 - 1.0, controls dither intensity
uniform int u_ColorDepth;              // Color levels per channel (e.g., 8, 16, 32 for retro look)
uniform float u_DitheringScale;        // Scale the dither pattern (1.0 = pixel perfect, 2.0 = larger pattern)

// Classic 4x4 Bayer ordered dithering matrix (normalized to 0-1 range)
const float bayerMatrix4x4[16] = float[16](
     0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
    12.0/16.0,  4.0/16.0, 14.0/16.0,  6.0/16.0,
     3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
    15.0/16.0,  7.0/16.0, 13.0/16.0,  5.0/16.0
);

// 8x8 Bayer matrix for finer dithering
const float bayerMatrix8x8[64] = float[64](
     0.0/64.0, 32.0/64.0,  8.0/64.0, 40.0/64.0,  2.0/64.0, 34.0/64.0, 10.0/64.0, 42.0/64.0,
    48.0/64.0, 16.0/64.0, 56.0/64.0, 24.0/64.0, 50.0/64.0, 18.0/64.0, 58.0/64.0, 26.0/64.0,
    12.0/64.0, 44.0/64.0,  4.0/64.0, 36.0/64.0, 14.0/64.0, 46.0/64.0,  6.0/64.0, 38.0/64.0,
    60.0/64.0, 28.0/64.0, 52.0/64.0, 20.0/64.0, 62.0/64.0, 30.0/64.0, 54.0/64.0, 22.0/64.0,
     3.0/64.0, 35.0/64.0, 11.0/64.0, 43.0/64.0,  1.0/64.0, 33.0/64.0,  9.0/64.0, 41.0/64.0,
    51.0/64.0, 19.0/64.0, 59.0/64.0, 27.0/64.0, 49.0/64.0, 17.0/64.0, 57.0/64.0, 25.0/64.0,
    15.0/64.0, 47.0/64.0,  7.0/64.0, 39.0/64.0, 13.0/64.0, 45.0/64.0,  5.0/64.0, 37.0/64.0,
    63.0/64.0, 31.0/64.0, 55.0/64.0, 23.0/64.0, 61.0/64.0, 29.0/64.0, 53.0/64.0, 21.0/64.0
);

// Get the dither threshold value at a given screen position
float GetBayerDither4x4(ivec2 pixelPos)
{
    int x = int(mod(float(pixelPos.x), 4.0));
    int y = int(mod(float(pixelPos.y), 4.0));

    return bayerMatrix4x4[y * 4 + x];
}

float GetBayerDither8x8(ivec2 pixelPos)
{
    int x = int(mod(float(pixelPos.x), 8.0));
    int y = int(mod(float(pixelPos.y), 8.0));

    return bayerMatrix8x8[y * 8 + x];
}

// Quantize a color channel to a specific number of levels
float QuantizeChannel(float value, float levels)
{
    return floor(value * levels + 0.5) / levels;
}

// Apply ordered dithering to a color
vec3 ApplyOrderedDithering(vec3 color, ivec2 pixelPos, float strength, int colorLevels)
{
    // Get dither threshold (centered around 0)
    float threshold = GetBayerDither4x4(pixelPos) - 0.5;

    // Calculate the step size based on color depth
    float levels = float(colorLevels);
    float stepSize = 1.0 / levels;

    // Apply dithering: add threshold before quantization
    vec3 dithered = color + vec3(threshold * stepSize * strength);

    // Quantize to limited color palette
    dithered.r = QuantizeChannel(dithered.r, levels);
    dithered.g = QuantizeChannel(dithered.g, levels);
    dithered.b = QuantizeChannel(dithered.b, levels);

    return clamp(dithered, 0.0, 1.0);
}

// Apply ordered dithering with 8x8 matrix (smoother gradients)
vec3 ApplyOrderedDithering8x8(vec3 color, ivec2 pixelPos, float strength, int colorLevels)
{
    float threshold = GetBayerDither8x8(pixelPos) - 0.5;
    float levels = float(colorLevels);
    float stepSize = 1.0 / levels;

    vec3 dithered = color + vec3(threshold * stepSize * strength);

    dithered.r = QuantizeChannel(dithered.r, levels);
    dithered.g = QuantizeChannel(dithered.g, levels);
    dithered.b = QuantizeChannel(dithered.b, levels);

    return clamp(dithered, 0.0, 1.0);
}

// PS1-style dithering with color banding (most authentic retro look)
vec3 ApplyPS1Dithering(vec3 color, ivec2 pixelPos)
{
    // PS1 had 15-bit color (5 bits per channel = 32 levels)
    const float PS1_COLOR_LEVELS = 32.0;

    // Get threshold and apply characteristic PS1 dither pattern
    float threshold = GetBayerDither4x4(pixelPos) - 0.5;
    float stepSize = 1.0 / PS1_COLOR_LEVELS;

    // The PS1 applied dithering more aggressively
    vec3 dithered = color + vec3(threshold * stepSize * 2.0);

    // Quantize to 15-bit color space
    dithered.r = floor(dithered.r * PS1_COLOR_LEVELS) / PS1_COLOR_LEVELS;
    dithered.g = floor(dithered.g * PS1_COLOR_LEVELS) / PS1_COLOR_LEVELS;
    dithered.b = floor(dithered.b * PS1_COLOR_LEVELS) / PS1_COLOR_LEVELS;

    return clamp(dithered, 0.0, 1.0);
}

// Main dithering function
vec3 ApplyDithering(vec3 color, vec2 screenCoord, vec2 resolution)
{
    if (!u_DitheringEnabled)
        return color;

    // Calculate pixel position (scale affects the dither pattern size)
    ivec2 pixelPos = ivec2(screenCoord / max(u_DitheringScale, 1.0));

    // Apply dithering based on settings
    return ApplyOrderedDithering(color, pixelPos, u_DitheringStrength, u_ColorDepth);
}

// Simplified version that uses authentic PS1 settings
vec3 ApplyPS1Effect(vec3 color, vec2 screenCoord)
{
    if (!u_DitheringEnabled)
        return color;

    ivec2 pixelPos = ivec2(screenCoord / max(u_DitheringScale, 1.0));
    return ApplyPS1Dithering(color, pixelPos);
}

