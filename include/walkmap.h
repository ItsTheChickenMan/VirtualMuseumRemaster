// player movement via walkmaps

#ifndef VMR_WALKMAP_H
#define VMR_WALKMAP_H

// includes //
#include <world.h>
#include <mouse.h>
#include <graphics.h>

// structs //

// map of keys to certain controls
struct Keymap {
	int32_t forward;
	int32_t backward;
	int32_t left;
	int32_t right;
	
	int32_t up;
	int32_t down;
	
	int32_t slow;
	int32_t fast;
};

struct Player {
	// camera
	PerspectiveCamera* camera;
	
	// map of keys corresponding to certain controls
	Keymap keymap;
	
	// last bounding box the player was standing on
	BoundingBox* currentBbox;
	
	// if the player is currently locked to an axis (by the edge of a bbox) then this is set to the axis, otherwise it's set to 0, 0
	glm::vec2 lockAxis;
};

// methods //
Player* createPlayer(PerspectiveCamera* camera, Keymap& keymap, Scene* scene);
glm::vec3 getMovementVector(Player* player, Window* window, float maxPlayerSpeed, double delta);
BoundingBox* checkBbox(BoundingBox* bbox, glm::vec3 oldPosition, glm::vec3 position, Scene* scene, std::vector<BoundingBox*>* checked, double distance, double delta, uint32_t* iterations);
void updatePlayerPosition(Player* player, Scene* scene, Window* window, double delta);
bool bboxContains(BoundingBox* box, glm::vec2 point);

#endif