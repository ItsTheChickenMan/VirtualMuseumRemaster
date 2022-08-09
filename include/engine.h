// museum engine manager

#ifndef VMR_ENGINE_H
#define VMR_ENGINE_H

// includes //
#include <audio.h>
#include <camera.h>
#include <graphics.h>
#include <mouse.h>
#include <shader.h>
#include <texture.h>
#include <utils.h>
#include <world.h>

// structs //
struct Player {
	PerspectiveCamera* camera;
	
	int32_t keymap[];
};

// methods //
void renderTexturedRenderableObject(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx);
void renderTexturedRenderableObjectNoBind(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx);

void renderScene(Scene* scene, PerspectiveCamera* camera, ShaderProgramEx* programEx);

#endif