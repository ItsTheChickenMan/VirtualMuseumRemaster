// light manager

#ifndef VMR_LIGHTING_H
#define VMR_LIGHTING_H

// includes //
#include <glm/glm.hpp>

// structs //

// point light
struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
	
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

// point light management
PointLight* createPointLight(glm::vec3 position, glm::vec3 color, float ambientStrength, float diffuseStrength, float c, float l, float q);

#endif