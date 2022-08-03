#version 330 core

layout (location=0) in vec3 vertexPosition;

out vec3 vertexPos;

uniform mat4 pvm;

void main(){
	gl_Position = pvm * vec4(vertexPosition, 1.0);

	vertexPos = vertexPosition;
}