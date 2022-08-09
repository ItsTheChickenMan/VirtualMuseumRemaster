// lighting
#include <lighting.h>
#include <utils.h>

// creates a point light
PointLight* createPointLight(glm::vec3 position, glm::vec3 color, float ambientStrength, float diffuseStrength, float c, float l, float q){
	PointLight* light = allocateMemoryForType<PointLight>();
	
	light->position = position;
	light->color = color;
	
	light->ambientStrength = ambientStrength;
	light->diffuseStrength = diffuseStrength;
	
	light->c = c;
	light->l = l;
	light->q = q;
	
	return light;
}