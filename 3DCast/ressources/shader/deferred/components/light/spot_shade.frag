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
    // Diffuse lighting
    vec3 lightDir = normalize(SLight.position - fragPos);
    float diffAngle = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffAngle * albedo * SLight.diffuse;

    // Specular lighting
    vec3 reflectDirection = reflect(-lightDir, normal);
    float specFac = pow(max(dot(viewDirection, reflectDirection), 0.0), shine);
    vec3 specular = SLight.specular * specFac * pixelSpecular;

    // Ambient lighting
    vec3 ambient = albedo * SLight.ambient;

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-SLight.direction)); 
    float epsilon = (SLight.outerCutOff - SLight.cutOff);
    float intensity = clamp((theta - SLight.cutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // Attenuation
    float dist = length(SLight.position - fragPos);
    float attenuation = 1.0 / (SLight.constant + SLight.linear * dist + SLight.quadratic * (dist * dist));

    // Apply attenuation and spotlight effect
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    ambient *= attenuation;

    return ambient + diffuse + specular;
}