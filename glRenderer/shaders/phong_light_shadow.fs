#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 lightSpacePos;

uniform vec3 viewPos;

struct Material {
	sampler2D texture_diffuse[8];
    sampler2D texture_specular[8];
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

// shadow map
uniform sampler2D depthMap;
uniform vec3 dirLightPos;

vec3 calcDirLight(DirectionalLight light, float shadow)
{
    // calculate diffuse factor
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    // calculate specular factor
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(2*dot(lightDir, norm) * norm - lightDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shiniess);

    vec3 ambient = vec3(0.0), diffuse = vec3(0.0), specular = vec3(0.0);
    
    for (int i=0; i<8; ++i)
    {
        ambient += light.ambient *vec3(texture(material.texture_diffuse[i], TexCoords));
        diffuse += light.diffuse * diff * vec3(texture(material.texture_diffuse[i], TexCoords));
        specular += light.specular * spec * vec3(texture(material.texture_specular[i], TexCoords));
    }

    return ambient + (1.f - shadow) * (diffuse + specular);
}

vec3 calcPointLight(PointLight pointLight, float shadow)
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

    vec3 ambient = vec3(0.0), diffuse = vec3(0.0), specular = vec3(0.0);

    for (int i=0; i<8; ++i)
    {
        ambient += pointLight.ambient * vec3(texture(material.texture_diffuse[i], TexCoords));
        diffuse += pointLight.diffuse * diff * vec3(texture(material.texture_diffuse[i], TexCoords));
        specular += pointLight.specular * spec * vec3(texture(material.texture_specular[i], TexCoords));
    }
    
    return attenuation * (ambient + (1.f - shadow) * (diffuse + specular));
}

vec3 calcFlashLight(FlashLight flashLight, float shadow) 
{
    vec3 lightDir = normalize(flashLight.position - FragPos);
    float theta = dot(lightDir, -normalize(flashLight.direction));
    float epsilon = flashLight.cutOff - flashLight.outerCutOff;
    float intensity = clamp((theta - flashLight.outerCutOff) / epsilon, 0.0, 1.0);

    //for (int i=0; i<8; ++i)
    //{
    //    ambient += flashLight.ambient * vec3(texture(material.texture_diffuse[i], TexCoords)); 
    //}
    vec3 color = (1.f - shadow) * intensity * flashLight.flash * vec3(texture(material.texture_diffuse[0], TexCoords));

    return color;
}

float calcShadow(vec4 lightSpacePos, DirectionalLight dirLight)
{
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w; // [-1, 1] (NDC)
    projCoords  = (projCoords + 1) * 0.5; // [0, 1]

    //float mapDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 lightNorm = normalize(-dirLight.direction);
    float bias = max(0.05 * (1.f - dot(lightNorm, Normal)), 0.005); // bias to add mapDepth

    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
  }

void main()
{
    vec3 color = vec3(0.0);
    
    float shadow = calcShadow(lightSpacePos, directionalLight);
    color += calcDirLight(directionalLight, shadow);
    for (int i=0; i<NUM_POINT_LIGHTS; ++i)
    {
        color += calcPointLight(pointLights[i], shadow); // todo: more accurate shadow 
    }
    color += calcFlashLight(flashLight, shadow); // todo : more accurate shadow
    FragColor = vec4(color, 1.f);
}