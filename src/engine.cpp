// museum engine manager

#include <engine.h>

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>

#include <filesystem>
#include <fstream>

// globals
const std::string g_gameInfoName = "gameinfo.json"; // path to game info

// assumes a texture uniform "texture1" exists in the shader
void renderTexturedRenderableObject(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx, bool noBind=false){
	// null checks
	if(!texturedRenderableObject || !camera || !programEx) return;
	
	// bind texture
	TextureData* textureData = texturedRenderableObject->textureData;
	
	// set color
	glUniform3fv(getProgramExUniformLocation(programEx, "color"), 1, glm::value_ptr(texturedRenderableObject->color));
	
	// set texture
	setProgramExUniformTexture(programEx, "texture1", textureData);
	
	// render
	RenderableObject* object = texturedRenderableObject->renderableObject;
	
	if(noBind){
		renderRenderableObjectNoBind(object, camera, programEx);
	} else {
		renderRenderableObject(object, camera, programEx);
	}
	
	// reset textures
	resetProgramExUniformTextures(programEx);
}

// render an entire scene
// assumes uniforms named pointLights, numPointLights, and normalMatrix exist
void renderScene(Scene* scene, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	float maxDistance2 = camera->far * camera->far;
	
	// add lights to shader
	for(uint32_t i = 0; i < scene->pointLights->size(); i++){
		PointLight* light = scene->pointLights->at(i);
		
		if(light != NULL) addProgramExPointLight(programEx, "pointLights", light);
	}
	
	uint32_t renderCalls = 0;
	
	// loop through vertex data
	for (std::map<VertexData*, std::vector<TexturedRenderableObject*>*>::iterator it = scene->staticObjects->begin(); it != scene->staticObjects->end(); it++){
    // bind vertex data
		bindVertexData(it->first);
		
		if(!it->second) continue;
		
		// render each object
		for(uint32_t i = 0; i < it->second->size(); i++){
			TexturedRenderableObject* object = it->second->at(i);
			
			if(!object || !object->visible) continue;
			
			// compute distance from camera
			
			// not perfect but good enough, basically creates a bounding sphere which can be ineffective for long thin objects
			float longestEdge2 = glm::length2(object->renderableObject->scale);
			
			glm::vec3 difVector = object->renderableObject->position - camera->position;
			float distance2 = glm::length2( difVector );
			
			distance2 -= longestEdge2;
			
			// if distance is greater than view distance, cull
			if(distance2 > maxDistance2) continue;
			
			// determine if object is absolutely behind viewer
			float longestEdge = sqrt(longestEdge2);
			
			glm::vec3 furthestPossiblePoint = object->renderableObject->position + camera->forward*longestEdge;
			
			float angle = glm::dot(camera->forward, glm::normalize(furthestPossiblePoint - camera->position) );
			
			// add a distance check because it's inconsistent with models at close range
			if(angle < 0 && distance2 > 64) continue;
			
			// set model matrix (necessary for lighting)
			glUniformMatrix4fv(getProgramExUniformLocation(programEx, "model"), 1, GL_FALSE, glm::value_ptr(object->renderableObject->modelMatrix));
			
			// set normal matrix (necessary for lighting)
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(object->renderableObject->modelMatrix)));
			glUniformMatrix3fv(getProgramExUniformLocation(programEx, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
			
			renderTexturedRenderableObject(object, camera, programEx, NO_BIND);
			
			renderCalls++;
		}
		
		// unbind vao
		glBindVertexArray(0);
	}
	
	// reset lights
	resetProgramExPointLights(programEx);
	
	//printf("render calls: %d\n", renderCalls);
}

// player related methods
// construct player
Player* createPlayer(PerspectiveCamera* camera, Keymap& keymap, float mouseSensitivity){
	Player* player = allocateMemoryForType<Player>();
	
	player->camera = camera;
	player->keymap = keymap;
	player->lockAxis = glm::vec2(0, 0);
	player->currentBbox = NULL;
	player->mouseSensitivity = mouseSensitivity;
	
	return player;
}

glm::vec3 getPlayerMovementVector(Player* player, Window* window, float maxPlayerSpeed, double delta){
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


glm::vec3 getPlayerRotationVector(float sensitivity){
	glm::vec2 delta = getMouseDelta();
	
	delta *= sensitivity;
	
	return glm::vec3(-delta.y, delta.x, 0);
}

// returns the bounding box the player is within, or null if the player isn't within any bboxes or adjacent bboxes
BoundingBox* checkBbox(BoundingBox* bbox, glm::vec3 oldPosition, glm::vec3 position, Scene* scene, std::vector<BoundingBox*>* checked, double delta, uint32_t* iterations){
	if(bbox == NULL) return NULL;
	
	if(iterations != NULL) (*iterations)++;
	
	// add to checked
	checked->push_back(bbox);

	// if player is in bbox, all good
	if(lineIntersectingBbox( glm::vec2(oldPosition.x, oldPosition.z), glm::vec2(position.x, position.z), bbox)){
		if(bboxContains(bbox, glm::vec2(position.x, position.z))){
			return bbox;
		} else {
			for(uint32_t i = 0; i < bbox->adjacent->size(); i++){
				BoundingBox* adjacent = scene->walkmap->at(bbox->adjacent->at(i));
				
				// make sure box hasn't been checked yet
				if(std::find(checked->begin(), checked->end(), adjacent) != checked->end()) continue;
			
				// check other
				BoundingBox* other = checkBbox(adjacent, oldPosition, position, scene, checked, delta, iterations);
				
				// if other was found in adjacent boxes, return it
				if(other != NULL) return other;
				
				// otherwise keep looking
			}
		}
	}
	
	// if none of the above adjacent box checks succeeded, return null
	return NULL;
}

// update player position from input
void updatePlayerPosition(Player* player, Scene* scene, Window* window, double delta){
	// get position of player given no interference
	glm::vec3 movementVector = getPlayerMovementVector(player, window, scene->maxPlayerSpeed, delta);
	
	glm::vec3 position = player->camera->position + movementVector;

	uint32_t iterations = 0;

	// check if player is within walkmap, if one exists
	if(scene->walkmap->size() > 0){
		std::vector<BoundingBox*> checked; // vector of bboxes already checked
		
		BoundingBox* playerBbox = checkBbox(player->currentBbox, player->camera->position, position, scene, &checked, delta, &iterations);
		
		// if playerBbox was found, update player bbox and call it a day
		if(playerBbox != NULL && player->currentBbox != NULL){
			player->currentBbox = playerBbox;
			player->lockAxis = glm::vec2(0, 0);
		} else {
			// correct player position
			
			// get corner lines
			glm::vec2 bbox2dPosition = glm::vec2(player->currentBbox->position.x, player->currentBbox->position.z);
			
			glm::vec2 ulLine = glm::normalize(player->currentBbox->UL - bbox2dPosition);
			glm::vec2 urLine = glm::normalize(player->currentBbox->UR - bbox2dPosition);
			glm::vec2 blLine = glm::normalize(player->currentBbox->BL - bbox2dPosition);
			// br line not needed since 3 lines is good enough w/ process of elimination
			
			// get line from center to player
			glm::vec2 playerLine = glm::normalize(glm::vec2(player->camera->position.x, player->camera->position.z) - bbox2dPosition);
			
			uint32_t lockAxisIndex = 0;
			
			if( vectorClockwise(playerLine, ulLine) ){
				lockAxisIndex = vectorClockwise(playerLine, urLine);
			} else {
				lockAxisIndex = vectorClockwise(playerLine, blLine);
			}
			
			glm::vec2 lockAxis = glm::vec2(!lockAxisIndex, lockAxisIndex);
			
			// TODO: any need to store the lock axis?
			player->lockAxis = lockAxis;
			
			printf("lockAxis: %f, %f\n", player->lockAxis.x, player->lockAxis.y);
			
			// project movement vector on lockAxis
			
			glm::vec2 mv2d = glm::vec2(movementVector.x, movementVector.z);
			
			float projectedLength = glm::dot(mv2d, player->lockAxis);
			
			glm::vec2 tempMv = player->lockAxis * projectedLength;
			
			movementVector.x = tempMv.x;
			movementVector.z = tempMv.y;
			
			position = player->camera->position + movementVector;
			
			// refetch player bbox
			// this is inefficient and it hurts to do
			// FIXME: extra bbox check
			checked.clear();
			
			playerBbox = checkBbox(player->currentBbox, player->camera->position, position, scene, &checked, delta, &iterations);
			
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
		float newHeight = currentHeight + (desiredHeight - currentHeight)* std::min(scene->heightSpeed * (float)delta, 1.0f);
		
		player->camera->position = glm::vec3(position.x, newHeight, position.z);
	} else {
		// no walkmap
		player->camera->position = position;
	}
	
	//printf("iterations: %d\n", iterations);
}

// create new player + current scene for a game
Game* createGame(Window* window, std::string gameDirString){
	// allocate memory for game
	Game* game = allocateMemoryForType<Game>();
	
	printf("Loading gameinfo.json...");
	
	// load game dir
	// convert gameDir to directory
	// FIXME: gameDir should probably be a directory from the start
	std::filesystem::path gameDir = std::filesystem::path(gameDirString);
	
	// load gameinfo.json
	// FIXME: switch to c++ io streams throughout the whole program
	std::filesystem::path dirBuffer = gameDir / g_gameInfoName;
	
	std::ifstream gameInfoBuffer = std::ifstream(dirBuffer.string().c_str());
	
	// make sure it exists
	if(!gameInfoBuffer.good()){
		printf("%s not found\n", dirBuffer.c_str());
		return NULL;
	}
	
	// convert buffer to json
	json gameInfo = json::parse(gameInfoBuffer, nullptr, true, true); // ignores comments
	
	// load camera settings
	float fov = gameInfo["settings"]["fov"];
	float near = gameInfo["settings"]["near"];
	float far = gameInfo["settings"]["far"];
	float mouseSensitivity = gameInfo["settings"]["mouseSensitivity"];
	
	json point = gameInfo["entry"]["point"];
	json rot = gameInfo["entry"]["rotation"];
	
	glm::vec3 startingPoint = glm::vec3(point[0], point[1], point[2]);
	glm::vec3 startingRotation = glm::vec3(glm::radians((float)rot[0]), glm::radians((float)rot[1]), glm::radians((float)rot[2]));
	
	// player creation //
	
	// create camera
	PerspectiveCamera* camera = createPerspectiveCamera(startingPoint, startingRotation, glm::radians(fov), (float)window->width, (float)window->height, near, far);
	
	// create player
	
	// TODO: keymap other than default?
	Keymap keymap = (Keymap){ GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_E, GLFW_KEY_Q };
	
	Player* player = createPlayer(camera, keymap, mouseSensitivity);
	
	printf("Done\nLoading entry scenes...");
	
	// load scene
	json startingWorlds = gameInfo["entry"]["worlds"];
	json startingWalkmaps = gameInfo["entry"]["walkmaps"];
	
	// create scene
	Scene* scene = createScene(window, gameDirString);
	
	std::filesystem::path worldDir = gameDir / "./worlds/";
	
	for(uint32_t i = 0; i < startingWorlds.size(); i++){
		parseWorldIntoScene(scene, (worldDir / startingWorlds[i]).string().c_str());
	}
	
	for(uint32_t i = 0; i < startingWalkmaps.size(); i++){
		parseWorldIntoScene(scene, (worldDir / startingWalkmaps[i]).string().c_str());
	}
	
	printf("Done\n");
	
	// player setup
	
	// determine currentBbox, if there's a walkmap
	if(scene->walkmap->size() > 0){
		for(uint32_t i = 0; i < scene->walkmap->size(); i++){
			if(bboxContains(scene->walkmap->at(i), glm::vec2(player->camera->position.x, player->camera->position.z))){
				player->currentBbox = scene->walkmap->at(i);
				
				break;
			}
		}
		
		if(player->currentBbox == NULL){
			player->currentBbox = scene->walkmap->at(0);
			player->camera->position = player->currentBbox->position; // height should correct itself
		}
	}
	
	game->player = player;
	game->currentScene = scene;
	game->gameDir = new std::string(gameDirString);
	
	return game;
}

// updates the game (does not render anything)
void updateGame(Game* game, double delta){
	// update player position (works regardless of walkmap presence)
	updatePlayerPosition(game->player, game->currentScene, game->currentScene->window, delta);
	
	// update player rotation
	glm::vec3 rotationVector = getPlayerRotationVector(game->player->mouseSensitivity);
	
	rotateCamera(game->player->camera, rotationVector);
	
	// constrain pitch to straight up/down
	constrainCameraRotation(game->player->camera, glm::vec3(glm::radians(-89.f), NO_LB, NO_LB), glm::vec3(glm::radians(89.f), NO_UB, NO_UB));
	
	// check scene triggers
	checkTriggers(game->currentScene, game->player);	
}