//include component

// structure
struct DirectionalLight{
    vec3 ambient;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

// shading
vec3 CalcDirectionalRadiance(DirectionalLight DirLight, vec3 normal, vec3 viewDirection){
    vec3 lightDir = normalize(-DirLight.direction);
    vec3 reflectDirection = reflect(-lightDir, normal);

    float diffAngle = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), u_shine[v_TexIndex]);

    vec3 diffuse = diffAngle * DirLight.diffuse * texture(u_Textures[v_TexIndex], v_TexCoord).rgb;
    vec3 specular = texture(u_SpecularMap[v_TexIndex], v_TexCoord).rgb * spec * DirLight.specular;
    vec3 ambient = DirLight.ambient * texture(u_Textures[v_TexIndex], v_TexCoord).rgb;

    return (diffuse + specular + ambient);
}