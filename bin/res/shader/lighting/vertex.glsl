#version 330 core

layout (location=0) in vec3 vertexPosition;
layout (location=1) in vec2 textureCoords;
layout (location=2) in vec3 normal;

// out
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

// uniforms
uniform mat4 pvm; // projection * view * model
uniform mat4 model; // just model
uniform mat3 normalMatrix; // matrix for adjusting normals for model matrix

void main(){
	gl_Position = pvm * vec4(vertexPosition, 1);

	TexCoords = textureCoords;
	Normal = normalMatrix * normalize(normal);
	FragPos = vec3(model * vec4(vertexPosition, 1));
}