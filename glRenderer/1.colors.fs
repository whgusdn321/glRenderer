#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiniess;
};
uniform Material material;

const float ambientStrength = 0.1;
const float specularStrength = 0.5;

void main()
{
    // ambient
    vec3 ambient = ambientStrength * material.ambient;
    // vec3 ambient = material.ambient;
    
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(2*dot(lightDir, norm) * norm - lightDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shiniess);
    vec3 specular = specularStrength * spec * material.specular;
    // vec3 specular = spec * material.specular;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}