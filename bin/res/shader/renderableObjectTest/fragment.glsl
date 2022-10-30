#version 330 core

in vec3 vertexPos;

out vec4 FragColor;

void main(){
	vec3 color = vertexPos+0.5f;
	
	FragColor = vec4(color, 1.0f);
}