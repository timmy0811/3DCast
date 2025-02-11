//include component

// structure
struct DirectionalLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// shading
vec3 AffectDirectionallight(DirectionalLight DirLight, vec3 normal, vec3 viewDirection, float shine, vec3 albedo, vec3 pixelSpecular){
    // specular
    DirLight.direction.y *= -1.0;
    vec3 lightDir = normalize(-DirLight.direction);
    vec3 reflectDirection = reflect(-lightDir, normal);
    float specFac = pow(max(dot(viewDirection, reflectDirection), 0.0), shine);
    vec3 specular = pixelSpecular * specFac * DirLight.specular;

    // diffuse
    float diffAngle = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffAngle * DirLight.diffuse * albedo;

    //ambient
    vec3 ambient = DirLight.ambient * albedo;

    return (diffuse + specular + ambient);
}