// world parser

#ifndef VMR_WORLD_H
#define VMR_WORLD_H

// includes //
#include <graphics.h>
#include <texture.h>
#include <lighting.h>

#include <string>
#include <vector>

// typdefs //

// both require a forward declaration of Scene and TriggerInfo
struct Scene;
struct TriggerInfo;

// event checker function
typedef bool (*EventCheckFunction)(Scene*, TriggerInfo*);

// trigger action function
typedef void (*TriggerActionFunction)(Scene*, TriggerInfo*);


// structs //

// textured renderable object
struct TexturedRenderableObject {
	TextureData* textureData;
	glm::vec3 color; // color if texture data is null
	
	RenderableObject* renderableObject;
	
	bool visible; // used by scene to only render "visible" objects, usually for debugging
};

// 2d bounding box struct
struct BoundingBox {
	// position and size
	glm::vec3 position;
	glm::vec2 size;
	
	// corners
	glm::vec2 UL, UR, BL, BR;
	
	// adjacent bounding box indexes
	std::vector<uint32_t>* adjacent;
};

// contains information on a trigger, which is used to check if the trigger needs to be fired and gets passed to the actual action when the trigger is fired
struct TriggerInfo {
	glm::vec3 position;
	glm::vec3 scale;
	
	std::vector<std::string>* strings;
	std::vector<float>* numbers;
	
	// the action that this trigger fires
	TriggerActionFunction action;
};

// scene
// contain static renderable objects
struct Scene {
	// loaded vertex data
	std::map<std::string, VertexData*>* vertexData;
	
	// loaded textures
	std::map<std::string, TextureData*>* textures;
	
	// models
	std::map<std::string, Model*>* models;
	
	// objects
	std::map<VertexData*, std::vector<TexturedRenderableObject*>*>* staticObjects;
	
	// lights
	std::vector<PointLight*>* pointLights;
	
	// walkmap
	std::vector<BoundingBox*>* walkmap;
	
	// triggers
	std::map<std::string, std::vector<TriggerInfo*>*>* triggers;
	
	// an offset for new walkmaps to adjust their adjacent indexes if a walkmap is loaded on top of this scene to avoid messing with adjacency in weird ways.  this gets updated at the end of every parseWorld and parseWorldIntoScene call
	uint32_t walkmapOffset;
	
	// the "frame" of the scene, although "number of times checkTriggers has been called" is more accurate
	uint32_t frame;
	
	// settings
	float playerHeight;
	float playerRadius;
	float stepHeight;
	float maxPlayerSpeed;
	
	// the rate at which updatePlayerPosition should approach the desired height
	// the formula for the player height given a desired height is:
	// currentHeight + (desiredHeight - currentHeight)*heightSpeed
	// the range of this number should be (0, 1]
	// a range of 1 is the same as matching the desiredHeight instantly each frame
	float heightSpeed;
};

struct Block {
	// vector for string parameters
	std::vector<std::string>* strings;
	
	// vector for num parameters (always floats)
	std::vector<float>* numbers;
	
	// index of parameter being written to
	uint32_t parameterIndex;
	
	// string buffer for parameter
	std::string* parameterBuffer;
};

// methods //

// trigger management
bool onStartChecker(Scene* scene, TriggerInfo* triggerInfo);
bool onEnterChecker(Scene* scene, TriggerInfo* triggerInfo);
bool onEnterRepeatChecker(Scene* scene, TriggerInfo* triggerInfo);

void logToConsole(Scene* scene, TriggerInfo* triggerInfo);
void changeSetting(Scene* scene, TriggerInfo* triggerInfo);
void playBackgroundMusic(Scene* scene, TriggerInfo* triggerInfo);

// other stuff
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, TextureData* texture);
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, glm::vec3 color);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, TextureData* texture);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color);
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, const char* texturePath);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const char* texturePath);

BoundingBox* createBbox(glm::vec2 p, glm::vec2 s);
BoundingBox* createBbox(glm::vec3 p, glm::vec2 s);
BoundingBox* createBbox(BoundingBox* original);

TriggerInfo* createTriggerInfo(glm::vec3 position, glm::vec3 scale, std::vector<std::string>* strings, std::vector<float>* numbers, std::string action);

Scene* createScene();
void parseWorldIntoScene(Scene* scene, const char* file);
Scene* parseWorld(const char* file);
bool hasWalkmap(Scene* scene);
void checkTriggers(Scene* scene);

#endif