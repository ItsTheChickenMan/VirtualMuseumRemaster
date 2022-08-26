// world parser
#include <world.h>
#include <utils.h>
#include <shapes.h>
#include <audio.h>

#include <cctype>

#include <stdexcept>
#include <ctgmath>
#include <algorithm>

#include <glm/gtx/norm.hpp>

// event types and action types are hard coded into the parser right here

// this map is for event names -> their event checker functions (the functions which check if a TriggerInfo struct should be fired)
// this should not be modified at all during runtime
const std::map<std::string, EventCheckFunction> g_eventCheckers = {
	{"onStart", onStartChecker},
	
	{"onEnter", onEnterChecker},
	
	{"onEnterRepeat", onEnterRepeatChecker},
	{"onEnterRepeating", onEnterRepeatChecker},
	
	{"onExit", onExitChecker},
	
	{"onKeyPress", onKeyPressChecker},
	
	{"onKeyHold", onKeyHoldChecker},
	
	{"onKeyRelease", onKeyReleaseChecker}
};

// this map is for storing action methods for each action name
const std::map<std::string, TriggerActionFunction> g_triggerActions = {
	{"logToConsole", logToConsole},
	{"changeSetting", changeSetting},
	{"playBackgroundMusic", playBackgroundMusicAction},
	{"setBackgroundMusicSettings", setBackgroundMusicSettings},
	{"playAudio", playAudio}
};


// trigger management //

// checker functions
// syntax = eventNameChecker
// note that in all checkers, 

bool onStartChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	return scene->frame == 0;
}

bool onEnterChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	// if this is the first frame, add some extra trigger related info to triggerInfo
	// this shouldn't cause any issues because triggers can't be created dynamically, so all valid triggers should be checked on the first checkTriggers call
	if(scene->frame == 0){
		// this value is equal to the last frame in which the player was intersecting the bounding box.  the trigger will never fire between two consecutive frames
		triggerInfo->reserved->push_back(0);
	}
	
	int32_t lastFrame = *triggerInfo->reserved->begin();
	
	if(inBoundingCube){
		// update last frame
		*triggerInfo->reserved->begin() = (int32_t)scene->frame;
		
		// if this frame and the last intersection frame are consecutive, return false, otherwise return true
		return scene->frame - lastFrame > 1;
	}
	
	return false;
}

bool onEnterRepeatChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	return inBoundingCube;
}

bool onExitChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	// works similar to onEnterChecker, but the opposite
	
	// if this is the first frame, add some extra trigger related info to triggerInfo
	if(scene->frame == 0){
		// this value is equal to the last frame in which the player was intersecting the bounding box.  the trigger will never fire between two consecutive frames
		triggerInfo->reserved->push_back(0);
	}
	
	int32_t lastFrame = *triggerInfo->reserved->begin();
	
	if(!inBoundingCube){
		// update last frame
		*triggerInfo->reserved->begin() = (int32_t)scene->frame;
		
		// if this frame and the last intersection frame are consecutive, return false, otherwise return true
		return scene->frame - lastFrame > 1;
	}
	
	return false;
}

bool onKeyPressChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	// initialization
	if(scene->frame == 0){
		triggerInfo->reserved->push_back(0);
	}
	
	int32_t lastFrame = *triggerInfo->reserved->begin();
	
	if(onKeyHoldChecker(scene, triggerInfo, inBoundingCube)){
		// update last frame
		*triggerInfo->reserved->begin() = (int32_t)scene->frame;
		
		// if this frame and the last press frame are consecutive, return false, otherwise return true
		return scene->frame - lastFrame > 1;
	}
	
	return false;
}

bool onKeyHoldChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	// size check
	if(triggerInfo->eventNumbers->size() < 1) return false;
	
	return glfwGetKey(scene->window->glfwWindow, (int32_t)triggerInfo->eventNumbers->at(0)) && inBoundingCube;
}

bool onKeyReleaseChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube){
	// initialization
	if(scene->frame == 0){
		triggerInfo->reserved->push_back(0);
	}
	
	int32_t lastFrame = *triggerInfo->reserved->begin();
	
	if(!onKeyHoldChecker(scene, triggerInfo, inBoundingCube)){
		// update last frame
		*triggerInfo->reserved->begin() = (int32_t)scene->frame;
		
		// if this frame and the last press frame are consecutive, return false, otherwise return true
		return scene->frame - lastFrame > 1;
	}
	
	return false;
}


// action functions

void logToConsole(Scene* scene, TriggerInfo* triggerInfo){
	for(uint32_t i = 0; i < triggerInfo->strings->size(); i++){
		if(i == 0){
			printf("%s", triggerInfo->strings->at(i).c_str());
		} else {
			printf(", %s", triggerInfo->strings->at(i).c_str());
		}
	}
	
	for(uint32_t i = 0; i < triggerInfo->numbers->size(); i++){
		if(i == 0 && triggerInfo->strings->size() == 0){
			printf("%f", triggerInfo->numbers->at(i));
		} else {
			printf(", %f", triggerInfo->numbers->at(i));
		}
	}
	
	printf("\n");
}

void changeSetting(Scene* scene, TriggerInfo* triggerInfo){
	// validate size
	if(triggerInfo->numbers->size() < 2){
		return;
	}
	
	// get setting index as int
	int32_t index = (int32_t)triggerInfo->numbers->at(0);
	
	// new setting
	float setting = triggerInfo->numbers->at(1);
	
	// modify setting
	(&scene->playerHeight)[index] = setting;
}

// we call this playBackgroundMusicAction because playBackgroundMusic already exists in audio.h
void playBackgroundMusicAction(Scene* scene, TriggerInfo* triggerInfo){
	// validate size
	// FIXME: it's not super efficient to validate every time this is fired when the string and numbers size never changes.  maybe do a separate validationCheck function per action that gets run every time a trigger block is created?  would avoid some other bugs caused by TriggerInfo holding initialization values as well (which could cause this to fail)
	if(triggerInfo->strings->size() < 2 || triggerInfo->numbers->size() < 1){
		return;
	}
	
	// load vals
	std::string& path = triggerInfo->strings->at(0);
	bool loop = triggerInfo->strings->at(1) == "true";
	float volume = triggerInfo->numbers->at(0);
	
	playBackgroundMusic(path, volume, loop);
}

void setBackgroundMusicSettings(Scene* scene, TriggerInfo* triggerInfo){
	// validate size
	if(triggerInfo->strings->size() < 1 || triggerInfo->numbers->size() < 1){
		return;
	}
	
	// load vals
	bool loop = triggerInfo->strings->at(0) == "true";
	float volume = triggerInfo->numbers->at(0);
	
	setBackgroundMusicLoop(loop);
	setBackgroundMusicVolume(volume);
}

void playAudio(Scene* scene, TriggerInfo* triggerInfo){
	// validate size
	if(triggerInfo->strings->size() < 3 || triggerInfo->numbers->size() < 1){
		return;
	}
	
	// load vals
	std::string key = triggerInfo->strings->at(0);
	float volume = triggerInfo->numbers->at(0);
	bool loop = triggerInfo->strings->at(1) == "true";
	bool spatial = triggerInfo->strings->at(2) == "true";
	
	// create a new sound
	sf::Sound* sound = createSound(key);
	
	if(sound == NULL) return;
	
	// update settings
	sound->setVolume(volume);
	sound->setLoop(loop);
	
	if(spatial){
		// load vals
		float attenuation = triggerInfo->numbers->size() > 1 ? triggerInfo->numbers->at(1) : 0.5f;
		float minDistance = triggerInfo->numbers->size() > 2 ? triggerInfo->numbers->at(2) : 1.f;
		
		sound->setPosition( glmVecToSFML(triggerInfo->position) );
		sound->setAttenuation(attenuation);
		sound->setMinDistance(minDistance);
	} else {
		sound->setRelativeToListener(true);
	}
	
	playSound(sound);
}

// others stuff //

// textured object constructors

// creates a textured renderable object from an existing object and an existing texture data
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, TextureData* texture){
	TexturedRenderableObject* texturedObject = allocateMemoryForType<TexturedRenderableObject>();
	
	texturedObject->renderableObject = object;
	texturedObject->textureData = texture;
	texturedObject->color = glm::vec3(0);
	texturedObject->visible = true;
	
	return texturedObject;
}

// creates a textured renderable object from an existing object and vec3 color
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, glm::vec3 color){
	TexturedRenderableObject* texturedObject = allocateMemoryForType<TexturedRenderableObject>();
	
	texturedObject->renderableObject = object;
	texturedObject->textureData = NULL;
	texturedObject->color = color;
	texturedObject->visible = true;
	
	return texturedObject;
}

// from new renderable object params and existing texture data
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, TextureData* texture){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	
	return createTexturedRenderableObject(object, texture);
}

// from new renderable object params and color
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	
	return createTexturedRenderableObject(object, color);
}

// from existing renderable object and new texture data params
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, const char* texturePath){
	TextureData* texture = createTextureData(texturePath);
	
	return createTexturedRenderableObject(object, texture);
}

// completely new
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const char* texturePath){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	TextureData* texture = createTextureData(texturePath);
	
	return createTexturedRenderableObject(object, texture);
}

// bounding box constructors

// create bounding box with 2d position
BoundingBox* createBbox(glm::vec2 p, glm::vec2 s){
	return createBbox(glm::vec3(p.x, 0, p.y), s);
}

// create bounding box with 3d position
BoundingBox* createBbox(glm::vec3 p, glm::vec2 s){
	// create 2d corners
	glm::vec2 UL = glm::vec2( (p.x-s.x/2.f), (p.z-s.y/2.f) );
	glm::vec2 UR = glm::vec2( (p.x+s.x/2.f), (p.z-s.y/2.f) );
	glm::vec2 BL = glm::vec2( (p.x-s.x/2.f), (p.z+s.y/2.f) );
	glm::vec2 BR = glm::vec2( (p.x+s.x/2.f), (p.z+s.y/2.f) );
	
	// allocate space for box
	BoundingBox* box = allocateMemoryForType<BoundingBox>();
	box->position = p;
	box->size = s;
	
	box->UL = UL;
	box->UR = UR;
	box->BL = BL;
	box->BR = BR;
	
	box->adjacent = new std::vector<uint32_t>();
	
	return box;
}

// copy bbox (but doesn't copy adjacent)
BoundingBox* createBbox(BoundingBox* original){
	return createBbox(original->position, original->size);
}

// destroy a bbox
void destroyBbox(BoundingBox* b){
	delete b->adjacent;
	
	free(b);
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

// check if two cubes are intersecting
bool cubesIntersecting(glm::vec3 p1, glm::vec3 s1, glm::vec3 p2, glm::vec3 s2){
	return (
		nearly_greater_or_eq(p1.x+s1.x/2.f, p2.x-s2.x/2.f) &&
		nearly_greater_or_eq(p2.x+s2.x/2.f, p1.x-s1.x/2.f) &&
		
		nearly_greater_or_eq(p1.y+s1.y/2.f, p2.y-s2.y/2.f) &&
		nearly_greater_or_eq(p2.y+s2.y/2.f, p1.y-s1.y/2.f) &&
		
		nearly_greater_or_eq(p1.z+s1.z/2.f, p2.z-s2.z/2.f) &&
		nearly_greater_or_eq(p2.z+s2.z/2.f, p1.z-s1.z/2.f)
	);
}

// construct player
Player* createPlayer(PerspectiveCamera* camera, Keymap& keymap){
	Player* player = allocateMemoryForType<Player>();
	
	player->camera = camera;
	player->keymap = keymap;
	player->lockAxis = glm::vec2(0, 0);
	player->currentBbox = NULL;
	
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
	
	glm::vec3 position = player->camera->position + movementVector;

	// check if player is within walkmap, if one exists
	if(scene->walkmap->size() > 0){
		std::vector<BoundingBox*> checked; // vector of bboxes already checked
		
		uint32_t iterations = 0;
		
		BoundingBox* playerBbox = checkBbox(player->currentBbox, player->camera->position, position, scene, &checked, 0, delta, &iterations);
		
		//printf("iterations: %d\n", iterations);
		
		// if playerBbox was found, update player bbox and call it a day
		if(playerBbox != NULL && player->currentBbox != NULL){
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

// world

// parser settings
const char commentDelimiter = '#';
const char parameterDelimiter = ',';
const char blockOpen = '[';
const char blockClose = ']';
const char textureBlockDelimiter = '%';
const char vertexDataBlockDelimiter = '*';
const char audioBlockDelimiter = '.';
const char objectBlockDelimiter = '$';
const char lightBlockDelimiter = '&';
const char modelBlockDelimiter = '+';
const char walkBoxBlockDelimiter = '~';
const char settingsBlockDelimiter = '@';
const char triggerBlockDelimiter = '!';

// create a block
Block* createBlock(){
	Block* block = allocateMemoryForType<Block>();
	
	// construct members
	block->strings = new std::vector<std::string>();
	block->numbers = new std::vector<float>();
	block->parameterBuffer = new std::string();
	
	block->parameterIndex = 0;
	block->parsingSubparameters = false;
	
	return block;
}

// empties out buffers and vectors, but keeps memory allocated
void emptyBlock(Block* block){
	// erase parameter buffer
	block->parameterBuffer->clear();
	
	// empty vectors
	block->strings->clear();
	block->numbers->clear();
	
	// reset parameter index
	block->parameterIndex = 0;
	
	block->parsingSubparameters = false;
}

// completely deletes occupied memory
void destroyBlock(Block* block){
	// destruct members
	delete block->strings;
	delete block->numbers;
	delete block->parameterBuffer;
	
	free(block);
}

// block char parser
bool blockCharParser(Block* block, char byte){
	// ensure that block exists, create it if it doesn't
	if(block == NULL){
		// weird problem, give up
		return false;
	}
	
	if(byte == '('){
		// prevent parameter buffer from flushing early
		block->parsingSubparameters = true;
	} else if(byte == ')'){
		// reenable flushing
		block->parsingSubparameters = false;
	}
	
	// add byte to parameter buffer if it's not equal to the parameter delimiter or the close block
	if( (byte != parameterDelimiter && byte != blockClose) || block->parsingSubparameters ){
		block->parameterBuffer->push_back(byte);
	} else {
		// flush parameter buffer
		if(block->parameterBuffer->length() > 0 && isStringNumber( *block->parameterBuffer )){
			block->numbers->push_back( std::stof(*block->parameterBuffer) );
		} else {
			// if string contains parenthesis, the parenthesis indicate sub-parameters
			// split the string at the first parenthesis, and push as two separate strings
			std::string::size_type parenthesisIndex = block->parameterBuffer->find_first_of('(');
			
			// found
			if(parenthesisIndex != std::string::npos){
				// push separately
				std::string original = block->parameterBuffer->substr(0, parenthesisIndex);
				std::string subparameters = block->parameterBuffer->substr(parenthesisIndex);
				
				block->strings->push_back(original);
				block->strings->push_back(subparameters);
			} else {
				block->strings->push_back( *block->parameterBuffer );
			}
		}
		
		// reset parameterBuffer
		block->parameterBuffer->clear();
		
		// if parameterIndex is at the maximum value, return true (done)
		if(byte == blockClose){
			// block is done parsing
			return true;
		}
		
		// otherwise, increment parameterIndex
		block->parameterIndex++;
	}
	
	return false;
}

// utility method to parse a block string into a block
// it's expected that the string is a single block ( [x,y,z,etc...] ), no delimiter
Block* parseStringToBlock(std::string blockString){
	Block* block = createBlock();
	
	uint32_t byteIndex = 1; // skip block open byte
	
	while(byteIndex < blockString.size()){
		char byte = blockString[byteIndex];
		byteIndex++;
		
		if(isspace(byte)) continue;
		
		blockCharParser(block, byte);
	}
	
	return block;
}


// block to scene methods
void textureBlockToScene(Block* block, Scene* scene){
	// load values
	std::string texturePath = block->strings->at(0);
	std::string textureName = block->strings->at(1);
	
	// exclude required special textures
	if(textureName == "invisible") return;
	
	// load texture
	(*scene->textures)[textureName] = createTextureData(texturePath.c_str());
}

void vertexDataBlockToScene(Block* block, Scene* scene){
	// load values
	std::string shapeName = block->strings->at(0);
	std::string vertexDataName = block->strings->at(1);
	
	// load vertex data
	VertexDataInfo info = g_shapes[shapeName];
	
	// FIXME: have createVertexData accept VertexDataInfo as a parameter instead of each individually (or both)
	(*scene->vertexData)[vertexDataName] = createVertexData(info.vertices, info.vertexCount, info.sizeInBytes, info.componentOrder, info.numComponents);
}

void objectBlockToScene(Block* block, Scene* scene){
	// load some float values
	if(block->numbers->size() < 9){
		printf("Not enough enough parameters in an object block (only %d numbers and %d strings present)\n", block->numbers->size(), block->strings->size());
		return;
	}
	
	float x = block->numbers->at(0);
	float y = block->numbers->at(1);
	float z = block->numbers->at(2);
	
	float rx = block->numbers->at(3);
	float ry = block->numbers->at(4);
	float rz = block->numbers->at(5);
	
	float w = block->numbers->at(6);
	float h = block->numbers->at(7);
	float d = block->numbers->at(8);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 rotation = glm::vec3(glm::radians(rx), glm::radians(ry), glm::radians(rz));
	glm::vec3 scale = glm::vec3(w, h, d);
	
	// check amount of strings
	uint32_t stringParams = block->strings->size();
	
	// first check if string params contains a special keyword
	// TODO: maybe write a better system for this?
	if(stringParams > 1){
		// loop breaks when there are no more keyword
		bool keywordsLeft = true;
		
		while(keywordsLeft){
			std::string keyword = block->strings->at(stringParams-1);
			
			if(keyword == "nowalk"){
				// nowalk is for walkmap parser only, so just get rid of the keyword and move on
				stringParams--;
			} else if(keyword == "invisible"){
				// ignore object completely
				// no cleanup necessary
				return;
			} else {
				keywordsLeft = false;
			}
		}
	}
	
	// mode depends on number of string parameters (1 = model, 2 = texture + vertexData)
	switch(stringParams){
		case 0: {
			printf("There weren't enough string parameters in an object block (only %d present)\n", block->strings->size());
			return;
		}
		
		case 1: {
			// load values
			std::string modelName = block->strings->at(0);
			
			// load model
			Model* model = (*scene->models)[modelName];
			
			// check model
			if(!model){
				// FIXME: fix the various potential memory leaks in these methods when exiting too early to delete heap memory
				printf("Invalid model name %s\n", modelName.c_str());
				return; // fail
			}
			
			// split model in textured renderable objects
			for(uint32_t i = 0; i < model->meshes->size(); i++){
				Mesh* mesh = model->meshes->at(i);
				
				// create renderable object from mesh
				RenderableObject* object = createRenderableObject(mesh->vertexData, position, rotation, scale);
				
				// create textured renderable object from renderable object and texture/color
				TexturedRenderableObject* texturedObject;
				
				if(mesh->texture){
					texturedObject = createTexturedRenderableObject(object, mesh->texture);
				} else {
					texturedObject = createTexturedRenderableObject(object, mesh->color);
				}
				
				// push to scene
				std::vector<TexturedRenderableObject*>* objectVector = (*scene->staticObjects)[mesh->vertexData];
				
				if(!objectVector){
					objectVector = new std::vector<TexturedRenderableObject*>();
					
					(*scene->staticObjects)[mesh->vertexData] = objectVector;
				}
				
				objectVector->push_back(texturedObject);
			}
			
			break;
		}
		case 2: {
			// load values
			std::string textureName = block->strings->at(0);
			std::string vertexDataName = block->strings->at(1);
			
			// check texture
			TextureData* texture = (*scene->textures)[textureName];
			
			if(!texture){
				printf("Invalid texture name %s", textureName.c_str());
				
				TextureData* def = (*scene->textures)["default"];
				
				if(def){
					texture = def;
					printf(", reverting to default\n");
				} else {
					printf(" with no default present\n");
					
					return; // fail
				}
			}
			
			// check vertex data
			VertexData* vData = (*scene->vertexData)[vertexDataName];
			
			if(!vData){
				printf("Invalid vertex data name %s\n", vertexDataName.c_str());
				
				return; // fail
			}
			
			// create object
			TexturedRenderableObject* object = createTexturedRenderableObject(vData, position, rotation, scale, texture);
			
			// push to scene
			std::vector<TexturedRenderableObject*>* objectVector = (*scene->staticObjects)[vData];
			
			if(!objectVector){
				objectVector = new std::vector<TexturedRenderableObject*>();
				
				(*scene->staticObjects)[vData] = objectVector;
			}
			
			objectVector->push_back(object);
			
			break;
		}
	}
}

void lightBlockToScene(Block* block, Scene* scene){
	// validate size
	if(block->numbers->size() < 11){
		printf("Not enough parameters for light block (only %d numbers present)\n", block->numbers->size());
		return;
	}
	
	// load values
	float x = block->numbers->at(0);
	float y = block->numbers->at(1);
	float z	= block->numbers->at(2);
	
	float r = block->numbers->at(3);
	float g = block->numbers->at(4);
	float b = block->numbers->at(5);
	
	float c = block->numbers->at(6);
	float l = block->numbers->at(7);
	float q = block->numbers->at(8);
	
	float as = block->numbers->at(9);
	float ds = block->numbers->at(10);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 color = glm::vec3(r, g, b);
	
	// create light
	PointLight* light = createPointLight(position, color, as, ds, c, l, q);
	
	// add to scene
	scene->pointLights->push_back(light);
}

void modelBlockToScene(Block* block, Scene* scene){
	// load values
	std::string path = block->strings->at(0);
	std::string modelName = block->strings->at(1);
	
	// load model
	Model* model = loadModel(path);
	
	if(!model){
		return; // fail
	}
	
	(*scene->models)[modelName] = model;
}

void walkBoxBlockToScene(Block* block, Scene* scene){
	// load values
	float x = block->numbers->at(0);
	float y = block->numbers->at(1);
	float z	= block->numbers->at(2);
	
	float w = block->numbers->at(3);
	float d = block->numbers->at(4);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec2 size = glm::vec2(w, d);
	
	// create bounding box
	BoundingBox* box = createBbox(position, size);
	
	// add adjacents
	for(uint32_t i = 5; i < block->numbers->size(); i++){
		// convert float to int index
		uint32_t index = (uint32_t)block->numbers->at(i) + scene->walkmapOffset;
		
		// add index to box adjacents
		box->adjacent->push_back(index);
	}
	
	// add to scene
	scene->walkmap->push_back(box);
}

void settingsBlockToScene(Block* block, Scene* scene){
	// block for misc settings
	
	// because memory is sequential in structs, this acts as an array of settings
	float* settings = &(scene->playerHeight);
	
	// loop through numbers and load them to settings
	for(uint32_t i = 0; i < block->numbers->size(); i++){
		settings[i] = block->numbers->at(i);
	}
}

void triggerBlockToScene(Block* block, Scene* scene){
	// validate size
	uint32_t numNums = 6;
	uint32_t numStrings = 2;
	if(block->numbers->size() < numNums || block->strings->size() < numStrings){
		printf("Not enough parameters in trigger block (only %d numbers and %d strings present when %d and %d were expected)\n", block->numbers->size(), block->strings->size(), numNums, numStrings);
		return;
	}
	
	// load float values
	glm::vec3 position;
	glm::vec3 scale;
	
	// first three floats should be position
	for(uint32_t i = 0; i < 3; i++){
		(&position.x)[i] = block->numbers->at(i);
	}
	
	// next three floats should be scale
	for(uint32_t i = 3; i < 6; i++){
		(&scale.x)[i-3] = block->numbers->at(i);
	}
	
	// load strings
	std::string event = block->strings->at(0);
	
	// check that event is valid
	if(!g_eventCheckers.count(event)){
		printf("Invalid event name %s\n", event.c_str());
		
		return;
	}
	
	// null unless there are event parameters
	Block* eventParametersBlock = NULL;
	std::vector<std::string>* eventStringParameters = NULL;
	std::vector<float>* eventNumberParameters = NULL;
	
	uint32_t actionsIndex = 1;
	
	// check for subparameters
	if(block->strings->at(1)[0] == '('){
		std::string eventParametersString = block->strings->at(1);
		
		// replace parenthesis with blockClose, to parse normally
		eventParametersString[eventParametersString.size()-1] = blockClose;
		
		eventParametersBlock = parseStringToBlock(eventParametersString);
		
		eventStringParameters = eventParametersBlock->strings;
		eventNumberParameters = eventParametersBlock->numbers;
		
		actionsIndex++;
	}
	
	std::string action = block->strings->at(actionsIndex);
	
	// check that action is valid
	if(!g_triggerActions.count(action)){
		printf("Invalid action name %s\n", action.c_str());
		
		return;
	}
	
	// get action subparameters
	std::string actionParametersString = block->strings->at(actionsIndex+1);
	
	// replace parenthesis with blockOpen/blockClose, to parse normally
	actionParametersString[actionParametersString.size()-1] = blockClose;
	
	Block* actionParametersBlock = parseStringToBlock(actionParametersString);
	
	// create trigger info
	// copies values from vectors, so no worries about bad memory here
	TriggerInfo* info = createTriggerInfo(position, scale, eventStringParameters, eventNumberParameters, actionParametersBlock->strings, actionParametersBlock->numbers, action);
	
	// store to scene triggers
	
	// construct vector for trigger if it doesn't exist
	std::vector<TriggerInfo*>* triggers = (*scene->triggers)[event];
	
	if(!triggers){
		triggers = new std::vector<TriggerInfo*>();
		
		(*scene->triggers)[event] = triggers;
	}
	
	triggers->push_back(info);
	
	// destroy blocks
	if(eventParametersBlock != NULL){
		destroyBlock(eventParametersBlock); // also destroys vectors
	}
	
	destroyBlock(actionParametersBlock);
}

void audioBlockToScene(Block* block, Scene* scene){
	// validate size
	uint32_t numStrings = 2;
	if(block->strings->size() < numStrings){
		printf("Not enough parameters for audio block (only %d strings present when %d were expected)\n", block->strings->size(), numStrings);
		return;
	}
	
	// load audio
	std::string path = block->strings->at(0);
	std::string name = block->strings->at(1);
	
	loadSoundFile(path, name);
}

// copies the elements from numbers and strings
TriggerInfo* createTriggerInfo(glm::vec3 position, glm::vec3 scale, std::vector<std::string>* eventStrings, std::vector<float>* eventNumbers, std::vector<std::string>* strings, std::vector<float>* numbers, std::string action){
	// check that action exists before doing anything
	if(!g_triggerActions.count(action)){
		printf("Invalid action %s for trigger info\n", action.c_str());
	}
	
	TriggerInfo* info = allocateMemoryForType<TriggerInfo>();
	
	info->position = position;
	info->scale = scale;
	info->eventStrings = eventStrings == NULL ? new std::vector<std::string>() : new std::vector<std::string>( *eventStrings );
	info->eventNumbers = eventNumbers == NULL ? new std::vector<float>() : new std::vector<float>( *eventNumbers );
	info->strings = new std::vector<std::string>( *strings );
	info->numbers = new std::vector<float>( *numbers );
	info->reserved = new std::vector<int32_t>();
	info->action = g_triggerActions.at(action);
	
	return info;
}

// create the shell of a scene
// also initializes player currentBbox
Scene* createScene(Window* window, Player* player){
	// allocate memory
	Scene* scene = allocateMemoryForType<Scene>();
	
	// initialize values
	scene->window = window;
	scene->player = player;
	scene->vertexData = new std::map<std::string, VertexData*>();
	scene->textures = new std::map<std::string, TextureData*>();
	scene->models = new std::map<std::string, Model*>();
	scene->staticObjects = new std::map<VertexData*, std::vector<TexturedRenderableObject*>*>();
	scene->pointLights = new std::vector<PointLight*>();
	scene->walkmap = new std::vector<BoundingBox*>();
	scene->triggers = new std::map<std::string, std::vector<TriggerInfo*>*>();
	scene->walkmapOffset = 0;
	scene->frame = 0;
	
	// default settings
	scene->playerHeight = 2.f;
	scene->playerRadius = 0.5f;
	scene->stepHeight = 0.4f;
	scene->maxPlayerSpeed = 2.f;
	scene->heightSpeed = 0.1f;
	
	return scene;
}

// parse a world file into an existing scene
void parseWorldIntoScene(Scene* scene, const char* file){
	// file buffer
	char* fileBuffer = read_entire_file(file);
	
	if(fileBuffer == NULL){
		printf("Invalid path for world %s\n", file);
		return;
	}
	
	// settings
	char blockDelimiters[] = {textureBlockDelimiter, vertexDataBlockDelimiter, objectBlockDelimiter, lightBlockDelimiter, modelBlockDelimiter, walkBoxBlockDelimiter, settingsBlockDelimiter, triggerBlockDelimiter, audioBlockDelimiter};
	
	// control states
	bool parsingBlock = false;
	bool ignoringUntilNextLine = false;
	int32_t blockParsing = -1;
	Block* blockBuffer = createBlock();
	
	// block parsers
	void (*blockParsers[9])(Block*,Scene*) {textureBlockToScene, vertexDataBlockToScene, objectBlockToScene, lightBlockToScene, modelBlockToScene, walkBoxBlockToScene, settingsBlockToScene, triggerBlockToScene, audioBlockToScene};
	
	// loop through each byte
	char byte;
	char lastByte;
	uint32_t byteIndex = 0;
	
	//printf("\n");
	
	do {
		// load byte
		byte = fileBuffer[byteIndex];
		if(byteIndex > 0) lastByte = fileBuffer[byteIndex-1];
		
		uint32_t index = byteIndex;
		byteIndex++;
		
		// if currently in a comment, check for newline
		if(ignoringUntilNextLine){
			// check for newline
			if(byte == '\n'){
				ignoringUntilNextLine = false;
			}
			
			continue;
		}
		
		// check for whitespace (ignored)
		if(isspace(byte)) continue;
		
		// check for hashtag (comment, ignores)
		if( byte == commentDelimiter ){
			ignoringUntilNextLine = true;
			continue;
		}
		
		// byte parser
		
		// if a block isn't being parsed currently, look for a delimiter
		if(blockParsing < 0){
			// look for delimiter
			for(int32_t i = 0; i < sizeof(blockDelimiters)/sizeof(char); i++){
				if(byte == blockDelimiters[i]){
					blockParsing = i;
					
					// skip block open
					byteIndex++;
					
					break;
				}
			}
			
			continue;
		}
		
		// if a block is being parsed, pass the byte to the block parser function
		bool done = (*blockCharParser)(blockBuffer, byte);
		
		if(done){
			// parse block into scene
			(*blockParsers[blockParsing])(blockBuffer, scene);
			
			// reset block parsing (blockBuffer is freed by block parser when done)
			blockParsing = -1;
			
			// empty the block
			emptyBlock(blockBuffer);
		}
	} while( byte != 0 ); // end at null terminator
	
	free(fileBuffer);
	
	destroyBlock(blockBuffer);
	
	// update walkmap offset
	scene->walkmapOffset = scene->walkmap->size();
	
	// player setup
	// determine currentBox if null
	if(scene->player->currentBbox == NULL && scene->walkmap->size() > 0){
		for(uint32_t i = 0; i < scene->walkmap->size(); i++){
			if(bboxContains(scene->walkmap->at(i), glm::vec2(scene->player->camera->position.x, scene->player->camera->position.z))){
				scene->player->currentBbox = scene->walkmap->at(i);
				
				break;
			}
		}
		
		if(scene->player->currentBbox == NULL){
			scene->player->currentBbox = scene->walkmap->at(0);
			scene->player->camera->position = scene->player->currentBbox->position; // height should correct itself
		}
	}
}

// parse world
Scene* parseWorld(const char* file, Window* window, Player* player){
	Scene* scene = createScene(window, player);
	
	parseWorldIntoScene(scene, file);
	
	return scene;
}

bool hasWalkmap(Scene* scene){
	return scene->walkmap->size() > 0;
}

// check triggers for scene
void checkTriggers(Scene* scene){
	// loop through every TriggerInfo
	for(std::map<std::string, std::vector<TriggerInfo*>*>::iterator it = scene->triggers->begin(); it != scene->triggers->end(); it++){
		// load checker function
		EventCheckFunction checker = g_eventCheckers.at(it->first);
		
		// loop through every trigger of this event
		for(uint32_t i = 0; i < it->second->size(); i++){
			TriggerInfo* triggerInfo = it->second->at(i);
			
			// check if we need to fire this trigger
			
			// intersecting is always true if the bounding cube's scale is all zeroes
			bool intersecting = glm::length2(triggerInfo->scale) == 0.0f || cubesIntersecting(glm::vec3(scene->player->camera->position.x, scene->player->camera->position.y - scene->playerHeight*0.375f, scene->player->camera->position.z), glm::vec3(scene->playerRadius, scene->playerHeight, scene->playerRadius), triggerInfo->position, triggerInfo->scale);
			bool fire = (*checker)(scene, triggerInfo, intersecting);
			
			if(fire){
				// fire at will
				(*triggerInfo->action)(scene, triggerInfo);
			}
		}
	}
	
	// increment scene frame
	scene->frame++;
}