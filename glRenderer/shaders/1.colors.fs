#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;  

uniform vec3 viewPos;

struct Material {
    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
    sampler2D diffuse;
    sampler2D specular;
    float shiniess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight { // ambient, diffuse, specular --> light(1, 1, 1) * intensity
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct FlashLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 flash;
};

#define NUM_POINT_LIGHTS 4
uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS]; 
uniform FlashLight flashLight;


vec3 calcDirLight(DirectionalLight light)
{
    // calculate diffuse factor
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    // calculate specular factor
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(2*dot(lightDir, norm) * norm - lightDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shiniess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight pointLight)
{
    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.f / (pointLight.constant + pointLight.linear*distance + pointLight.quadratic*(distance*distance));

    // calculate diffuse factor
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // calculate specular factor
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(2*dot(lightDir, norm) * norm - lightDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shiniess);

    vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = pointLight.specular * spec * vec3(texture(material.specular, TexCoords));
    
    return attenuation * (ambient + diffuse + specular);
}

vec3 calcFlashLight(FlashLight flashLight) 
{
    vec3 lightDir = normalize(flashLight.position - FragPos);
    float theta = dot(lightDir, -normalize(flashLight.direction));
    float epsilon = flashLight.cutOff - flashLight.outerCutOff;
    float intensity = clamp((theta - flashLight.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = flashLight.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 color = intensity * flashLight.flash * vec3(texture(material.diffuse, TexCoords));

    return color;
}
void main()
{
    vec3 color = vec3(0.0);
    color += calcDirLight(directionalLight);
    for (int i=0; i<NUM_POINT_LIGHTS; ++i)
    {
        color += CalcPointLight(pointLights[i]);
    }
    color += calcFlashLight(flashLight);
    FragColor = vec4(color, 1.f);
}