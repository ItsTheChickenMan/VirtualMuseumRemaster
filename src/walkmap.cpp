// player movement via walkmaps

#include <walkmap.h>
#include <utils.h>

#include <ctgmath>
#include <algorithm>

#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

// construct player
Player* createPlayer(PerspectiveCamera* camera, Keymap& keymap, Scene* scene){
	Player* player = allocateMemoryForType<Player>();
	
	player->camera = camera;
	player->keymap = keymap;
	player->lockAxis = glm::vec2(0, 0);
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
	
	if(iterations != NULL) (*iterations)++;
	
	// add to checked
	checked->push_back(bbox);
	
	/*for(std::map<VertexData*, std::vector<TexturedRenderableObject*>*>::iterator it = scene->staticObjects->begin(); it != scene->staticObjects->end(); it++){
		it->second->at(std::find(scene->walkmap->begin(), scene->walkmap->end(), bbox) - scene->walkmap->begin())->visible = true;	
	}*/

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
			//newDistance = 0;
			
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
	glm::vec3 movementVector = getMovementVector(player, window, scene->maxPlayerSpeed, delta);
	
	// adjust movement vector if locked to an axis
	/*if(glm::length2(player->lockAxis) > 0){
		// project movement vector along axis through dot product
		float mvFactor = glm::dot(player->lockAxis, glm::normalize(glm::vec2(movementVector.x, movementVector.z)));
		
		glm::vec2 tempMv = player->lockAxis * mvFactor * scene->maxPlayerSpeed * (float)delta;
		
		movementVector.x = tempMv.x;
		movementVector.z = tempMv.y;
	}*/
	
	glm::vec3 position = player->camera->position + movementVector;
	
	/*for (std::map<VertexData*, std::vector<TexturedRenderableObject*>*>::iterator it = scene->staticObjects->begin(); it != scene->staticObjects->end(); it++){
		it->second->at(std::find(scene->walkmap->begin(), scene->walkmap->end(), player->currentBbox) - scene->walkmap->begin())->visible = true;
	}*/
	
	// check if player is within walkmap, if one exists
	if(scene->walkmap->size() > 0){
		std::vector<BoundingBox*> checked; // vector of bboxes already checked
		
		uint32_t iterations = 0;
		
		BoundingBox* playerBbox = checkBbox(player->currentBbox, player->camera->position, position, scene, &checked, 0, delta, &iterations);
		
		//printf("iterations: %d\n", iterations);
		
		// if playerBbox was found, update player bbox and call it a day
		if(playerBbox != NULL){
			player->currentBbox = playerBbox;
			player->lockAxis = glm::vec2(0, 0);
		} else {
			// correct player position
			// FIXME: the math below is overly complex and can/should be simplified + the extra bbox check isn't fun
			
			// first determine which side of the bounding box the player is colliding with
			if(glm::length2(player->lockAxis) == 0){
				bool temp = position.x < player->currentBbox->UL.x || position.x > player->currentBbox->UR.x;
				glm::vec2 axis = glm::vec2(!temp, !!temp);
				
				player->lockAxis = axis;
			}
			
			// project movement vector along axis through dot product
			float mvFactor = glm::dot(player->lockAxis, glm::normalize(glm::vec2(movementVector.x, movementVector.z)));
			
			glm::vec2 tempMv = player->lockAxis * mvFactor * scene->maxPlayerSpeed * (float)delta;
			
			movementVector.x = tempMv.x;
			movementVector.z = tempMv.y;
			
			position = player->camera->position + movementVector;
			
			// refetch player bbox
			// this is inefficient and it hurts to do
			checked.clear();
			
			playerBbox = checkBbox(player->currentBbox, player->camera->position, position, scene, &checked, 0, delta, NULL);
			
			if(!playerBbox){
				position = player->camera->position;
			} else {
				player->currentBbox = playerBbox;
			}
			
			//printf("player exiting\n");
		}
		
		// update position
		// we make the camera 7/8 of player height to leave a bit of headroom and not look through objects above
		float currentHeight = player->camera->position.y;
		float desiredHeight = player->currentBbox->position.y + scene->playerHeight*0.875;
		float newHeight = currentHeight + (desiredHeight - currentHeight)*scene->heightSpeed;
		
		player->camera->position = glm::vec3(position.x, newHeight, position.z);
	} else {
		// no walkmap
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