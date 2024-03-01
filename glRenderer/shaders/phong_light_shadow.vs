#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 lightSpacePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	gl_Position = projection * view * worldPos;
	lightSpacePos = lightSpaceMatrix * worldPos;
	Normal = mat3(transpose(inverse(model))) * aNormal;  
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoords;
}