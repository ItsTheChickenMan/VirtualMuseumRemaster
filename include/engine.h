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

// TODO: forward defs
#include <nlohmann/json.hpp>

// macros + typedefs //
#define NO_BIND true
#define BIND false

typedef nlohmann::json json;

// structs //

// manages player interaction with a scene
struct Game {
	Player* player;
	
	Scene* currentScene;
	
	std::string* gameDir;
};

// methods //

void renderTexturedRenderableObject(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx, bool noBind/*=false*/);

void renderScene(Scene* scene, PerspectiveCamera* camera, ShaderProgramEx* programEx);

Player* createPlayer(PerspectiveCamera* camera, Keymap& keymap);
glm::vec3 getPlayerMovementVector(Player* player, Window* window, float maxPlayerSpeed, double delta);
glm::vec3 getPlayerRotationVector(float sensitivity);

BoundingBox* checkBbox(BoundingBox* bbox, glm::vec3 oldPosition, glm::vec3 position, Scene* scene, std::vector<BoundingBox*>* checked, double delta, uint32_t* iterations);
void updatePlayerPosition(Player* player, Scene* scene, Window* window, double delta);

Game* createGame(Window* window, std::string gameDirString);
void updateGame(Game* game, double delta);

#endif