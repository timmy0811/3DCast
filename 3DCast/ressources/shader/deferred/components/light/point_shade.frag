//include component

// structure
struct PointLight{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// shading
vec3 AffectPointlight(PointLight PLight, vec3 normal, vec3 viewDirection, vec3 fragPos, float shine, vec3 albedo, vec3 pixelSpecular){
    // ambient
    vec3 ambient = PLight.ambient * albedo;

    //diffuse
    vec3 lightDir = normalize(PLight.position - fragPos);
    float diffAngle = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffAngle * PLight.diffuse * albedo;

    // specular
    vec3 reflectDirection = reflect(-lightDir, normal);
    float specFac = pow(max(dot(viewDirection, reflectDirection), 0.0), shine);
    vec3 specular = pixelSpecular * specFac * PLight.specular;

    // attenuation
    float dist = length(PLight.position - fragPos);
    float attenuation = 1.0 / (PLight.constant + PLight.linear * dist + PLight.quadratic * (dist + dist));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}