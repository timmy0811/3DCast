//include component

// Fog uniforms
uniform bool u_FogEnabled;
uniform vec3 u_FogColorBottom;      // Lower color of the fog gradient (below horizon)
uniform vec3 u_FogColorTop;         // Upper color of the fog gradient (above horizon)
uniform float u_FogStartDistance;   // Distance where fog begins
uniform float u_FogEndDistance;     // Distance where fog fully tints geometry
uniform float u_FogAngleMin;        // Vertical angle (degrees) where bottom color applies (e.g., -45)
uniform float u_FogAngleMax;        // Vertical angle (degrees) where top color applies (e.g., 45)
uniform float u_FogDensity;         // Density factor for exponential fog (higher = thicker fog)
uniform bool u_FogUseExponential;   // true = exponential falloff, false = linear

// Calculate fog factor based on distance from camera
float CalculateFogFactor(float distance)
{
    if (distance <= u_FogStartDistance)
        return 0.0;

    if (distance >= u_FogEndDistance)
        return 1.0;

    // Normalized distance between start and end
    float t = (distance - u_FogStartDistance) / (u_FogEndDistance - u_FogStartDistance);

    if (u_FogUseExponential)
    {
        // Using formula: (1 - e^(-steepness * t)) / (1 - e^(-steepness))
        // 0 at t=0 and 1 at t=1
        float expFactor = 1.0 - exp(-u_FogDensity * t);
        float normalization = 1.0 - exp(-u_FogDensity);
        return expFactor / normalization;
    }
    else
    {
        // Linear fog
        return t;
    }
}

// Calculate fog color based on vertical viewing angle from camera to fragment
vec3 GetFogColorByAngle(vec3 viewDirection)
{
    float verticalAngle = degrees(asin(viewDirection.y));

    // Normalized angle
    float angleFactor = clamp((verticalAngle - u_FogAngleMin) / max(u_FogAngleMax - u_FogAngleMin, 0.001), 0.0, 1.0);

    return mix(u_FogColorBottom, u_FogColorTop, angleFactor);
}

// Apply fog to a color based on fragment world position and camera position
vec3 ApplyFog(vec3 originalColor, vec3 fragPos, vec3 viewPos)
{
    if (!u_FogEnabled)
        return originalColor;

    vec3 toFragment = fragPos - viewPos;
    float distance = length(toFragment);

    float fogFactor = CalculateFogFactor(distance);
    vec3 viewDirection = toFragment / distance;
    vec3 fogColor = GetFogColorByAngle(viewDirection);

    return mix(originalColor, fogColor, fogFactor);
}

