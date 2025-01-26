//include component

// structure
struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
    float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

// shading
vec3 CalcSpotRadiance(SpotLight SLight, vec3 normal){
    vec3 lightDir = normalize(SLight.position - v_FragPos);

    // Calculate Spotlight
    float theta = dot(lightDir, normalize(-SLight.direction));
    float epsilon = SLight.cutOff - SLight.outerCutOff;
    float intensity = clamp((theta - SLight.outerCutOff) / epsilon, 0.0, 1.0);

    // Light Direction Calculation
    float distance = length(SLight.position - v_FragPos);
    float attenuation = 1.0 / (SLight.constant + SLight.linear * distance + SLight.quadratic * (distance + distance));
    float diffAngle = max(dot(normal, lightDir), 0.0);

    vec3 diffuse = (diffAngle * texture(u_Textures[v_TexIndex], v_TexCoord).rgb) * SLight.diffuse;

    // Specular Lighting
    vec3 viewDirection = normalize(u_ViewPosition - v_FragPos);
    vec3 reflectDirection = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), u_shine[v_TexIndex]);
    vec3 specular = SLight.specular * (spec * texture(u_SpecularMap[v_TexIndex], v_TexCoord).rgb);

    // Ambient light
    vec3 ambient = texture(u_Textures[v_TexIndex], v_TexCoord).rgb * SLight.ambient;

    specular *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    ambient *= attenuation;

    return (ambient + diffuse + specular);
}