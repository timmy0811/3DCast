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
vec3 CalcPointRadiance(PointLight PLight, vec3 normal, vec3 viewDirection, vec3 fragPos){
    vec3 lightDir = normalize(PLight.position - fragPos);
    float diffAngle = max(dot(normal, lightDir), 0.0);

    vec3 reflectDirection = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), u_shine[v_TexIndex]);

    float distance = length(PLight.position - fragPos);
    float attenuation = 1.0 / (PLight.constant + PLight.linear * distance + PLight.quadratic * (distance + distance));

    vec3 ambient = PLight.ambient * texture(u_Textures[v_TexIndex], v_TexCoord).rgb;
    vec3 diffuse = diffAngle * PLight.diffuse * texture(u_Textures[v_TexIndex], v_TexCoord).rgb;
    vec3 specular = texture(u_SpecularMap[v_TexIndex], v_TexCoord).rgb * spec * PLight.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}