#version 330 core

layout (location=0) in vec3 vertexPosition;
layout (location=1) in vec2 textureCoords;

out vec2 TexCoords;

uniform mat4 pvm;

void main(){
	gl_Position = pvm * vec4(vertexPosition, 1.0f);

	TexCoords = textureCoords;
}