//include component

// structure
struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
    
	float cutOff;
    float outerCutOff;
};

// shading
vec3 AffectSpotlight(SpotLight SLight, vec3 normal, vec3 fragPos, vec3 viewDirection, vec3 albedo, float shine, vec3 pixelSpecular) {
    vec3 lightDir = normalize(SLight.position - fragPos);

    // Spotlight intensity (soft transition)
    float theta = dot(lightDir, normalize(SLight.direction)); 
    float intensity = smoothstep(SLight.outerCutOff, SLight.cutOff, theta);

    // Distance-based attenuation
    float dist = length(SLight.position - fragPos);
    float attenuation = 1.0 / (SLight.constant + SLight.linear * dist + SLight.quadratic * (dist * dist));

    // Diffuse lighting
    float diffAngle = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffAngle * albedo * SLight.diffuse;

    // Specular lighting
    vec3 reflectDirection = reflect(-lightDir, normal);
    float specFac = pow(max(dot(viewDirection, reflectDirection), 0.0), shine);
    vec3 specular = SLight.specular * specFac * pixelSpecular;

    // Ambient lighting
    vec3 ambient = albedo * SLight.ambient;

    // Apply attenuation and spotlight effect
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    ambient *= attenuation;

    return ambient + diffuse + specular;
}
