// player movement via walkmaps

#include <walkmap.h>
#include <utils.h>

#include <ctgmath>
#include <algorithm>

#include <glm/gtx/norm.hpp>

// construct player
Player* createPlayer(PerspectiveCamera* camera, Keymap& keymap, Scene* scene){
	Player* player = allocateMemoryForType<Player>();
	
	player->camera = camera;
	player->keymap = keymap;
	player->currentBbox = NULL;
	
	// determine currentBox
	if(scene->walkmap->size() > 0){
		for(uint32_t i = 0; i < scene->walkmap->size(); i++){
			if(bboxContains(scene->walkmap->at(i), glm::vec2(camera->position.x, camera->position.z))){
				player->currentBbox = scene->walkmap->at(i);
				
				break;
			}
		}
		
		if(player->currentBbox == NULL){
			player->currentBbox = scene->walkmap->at(0);
			player->camera->position = player->currentBbox->position; // height should correct itself
		}
	}
	
	return player;
}

glm::vec3 getMovementVector(Player* player, Window* window, float maxPlayerSpeed, double delta){
	glm::vec3 forward = glm::vec3(cos(player->camera->rotation.y), 0, sin(player->camera->rotation.y));
	glm::vec3 side = glm::normalize( glm::cross(forward, player->camera->up) );
	
	glm::vec3 directions[] = {forward, -forward, -side, side, player->camera->up, -player->camera->up};
	
	glm::vec3 movement = glm::vec3(0);
	
	// loop through 2d keys
	for(uint32_t i = 0; i < sizeof(directions)/sizeof(glm::vec3); i++){
		int32_t key = (&player->keymap.forward)[i];
		
		if(glfwGetKey(window->glfwWindow, key) == GLFW_PRESS){
			movement += directions[i];
		}
	}
	
	if( glm::length2(movement) == 0 ) return movement;
	
	movement = glm::normalize(movement) * maxPlayerSpeed * (float)delta;
	
	return movement;
}

// returns the bounding box the player is within, or null if the player isn't within any bboxes or adjacent bboxes
BoundingBox* checkBbox(BoundingBox* bbox, glm::vec3 oldPosition, glm::vec3 position, Scene* scene, std::vector<BoundingBox*>* checked, double distance, double delta, uint32_t* iterations){
	if(bbox == NULL) return NULL;
	
	(*iterations)++;
	
	// add to checked
	checked->push_back(bbox);
	
	// if player is in bbox, all good
	if(bboxContains(bbox, glm::vec2(position.x, position.z))){
		return bbox;
	} else {
		// if the distance exceeds the maximum expected distance the player could travel, stop checking boxes and return NULL
		double maxDistance = scene->maxPlayerSpeed*delta;
		
		if(distance > maxDistance) return NULL;
		
		for(uint32_t i = 0; i < bbox->adjacent->size(); i++){
			BoundingBox* adjacent = scene->walkmap->at(bbox->adjacent->at(i));
			
			// make sure box hasn't been checked yet
			if(std::find(checked->begin(), checked->end(), adjacent) != checked->end()) continue;
			
			float newDistance = distance + (double)glm::length( glm::vec2(oldPosition.x, oldPosition.z) - glm::vec2(adjacent->position.x, adjacent->position.z) );
			
			// check other
			BoundingBox* other = checkBbox(adjacent, oldPosition, position, scene, checked, newDistance, delta, iterations);
			
			// if other was found in adjacent boxes, return it
			if(other != NULL) return other;
			
			// otherwise keep looking
		}
	}
	
	// if none of the above adjacent box checks succeeded, return null
	return NULL;
}

// update player position from input
void updatePlayerPosition(Player* player, Scene* scene, Window* window, double delta){
	// get position of player given no interference
	glm::vec3 position = player->camera->position + getMovementVector(player, window, scene->maxPlayerSpeed, delta);
	
	// check if player is within walkmap, if one exists
	if(scene->walkmap->size() > 0){
		std::vector<BoundingBox*> checked; // vector of bboxes already checked
		
		uint32_t iterations = 0;
		
		BoundingBox* playerBbox = checkBbox(player->currentBbox, player->camera->position, position, scene, &checked, 0, delta, &iterations);
		
		// if playerBbox was found, update player bbox and call it a day
		if(playerBbox != NULL){
			player->currentBbox = playerBbox;
			
			player->camera->position = glm::vec3(position.x, player->currentBbox->position.y + scene->playerHeight, position.z);
		} else {
			printf("player exiting\n");
		}
	} else {
		player->camera->position = position;
	}
}

// check if bbox contains point
bool bboxContains(BoundingBox* bbox, glm::vec2 point){
	return (
		point.x >= bbox->position.x-bbox->size.x/2.f &&
		point.x <= bbox->position.x+bbox->size.x/2.f &&
		point.y >= bbox->position.z-bbox->size.y/2.f &&
		point.y <= bbox->position.z+bbox->size.y/2.f
	);
}