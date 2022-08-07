// museum engine manager

#ifndef VMR_ENGINE_H
#define VMR_ENGINE_H

#include <graphics.h>

// scene
// contain static renderable objects
struct Scene {
	RenderableObject* staticObjects;
	
	uint32_t staticObjectCount;
};

#endif