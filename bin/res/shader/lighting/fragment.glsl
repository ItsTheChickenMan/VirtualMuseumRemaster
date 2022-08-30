#version 330 core

// structs

// point light
struct PointLight {
	vec3 position;
	vec3 color;
	
	// strength of sources
	float ambientStrength;
	float diffuseStrength;
	
	// attenuation values
	float c;
	float l;
	float q;
	
	// range?
	// float range;
};


// in
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

// out
out vec4 FragColor;

// lights
#define MAX_POINT_LIGHTS 48
uniform PointLight[MAX_POINT_LIGHTS] pointLights;
uniform int numPointLights;

// texture
uniform sampler2D texture1;

// color (defined if no texture is defined)
uniform vec3 color;

// on some implementations opengl doesn't consider the pointLights active uniforms, so we "wake them up" by accessing their properties once
// this is very annoying, I'd love to find a fix but it doesn't seem like anyone else has encountered this problem (at least on SO)
void wakeUpPointLights();

vec3 calculatePointLightContribution(PointLight light);

void main(){
	wakeUpPointLights();

	// get texture sample
	vec3 baseColor = vec3(texture(texture1, TexCoords)) + color;
	
	vec3 final = vec3(0);
	//final = baseColor;
	
	// calculate lighting
	for(int i = 0; i < numPointLights; i++){
		PointLight light = pointLights[i];
		
		vec3 contribution = calculatePointLightContribution(light);
		
		final += baseColor*contribution;
	}
	
	FragColor = vec4(final, 1);
}

void wakeUpPointLights(){
	pointLights[0].position;
	pointLights[0].color;
	pointLights[0].ambientStrength;
	pointLights[0].diffuseStrength;
	pointLights[0].c;
	pointLights[0].l;
	pointLights[0].q;
}

// expects normalized values when necessary
vec3 calculatePointLightContribution(PointLight light){
	// calculate ambient
	float ambient = light.ambientStrength;
	
	// calculate diffuse
	vec3 lightRay = light.position-FragPos;
	float distance = length(lightRay); // used for attenuation
	lightRay = normalize(lightRay);
	
	//float diffuse = max(dot(Normal, lightRay), 0);
	float diffuse = max(dot(Normal, lightRay), 0);
	//float diffuse = dot(Normal, lightRay);
	//float diffuse = abs(dot(Normal, lightRay)); // fun line
	
	// calculate attenuation
	float attenuation = 1 / (light.c + light.l * distance + light.q*distance*distance);
	
	// only attenuate diffuse light
	diffuse *= attenuation;
	
	// apply diffuse factor
	diffuse *= light.diffuseStrength;
	
	// calculate total contribution
	vec3 contribution = light.color * (ambient+diffuse);
	
	return contribution;
}