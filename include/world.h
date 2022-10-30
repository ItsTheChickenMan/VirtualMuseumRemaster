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
typedef bool (*EventCheckFunction)(Scene*, TriggerInfo*, bool);

// trigger action function
typedef void (*TriggerActionFunction)(Scene*, TriggerInfo*);


// structs & unions //

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

// textured renderable object
struct TexturedRenderableObject {
	TextureData* textureData;
	glm::vec3 color; // color if texture data is null
	
	RenderableObject* renderableObject;
	
	bool visible; // used by scene to only render "visible" objects, usually for debugging
};

// contains information on a trigger, which is used to check if the trigger needs to be fired and gets passed to the actual action when the trigger is fired
struct TriggerInfo {
	glm::vec3 position;
	glm::vec3 scale;
	
	// string/number parameters for the event (or the checker)
	std::vector<std::string>* eventStrings;
	std::vector<float>* eventNumbers;
	
	std::vector<std::string>* strings;
	std::vector<float>* numbers;
	
	// reserved data for any use that the particular type of TriggerInfo needs
	std::vector<int32_t>* reserved;
	
	// the action that this trigger fires
	TriggerActionFunction action;
};

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

// contains information required for a player to interact
struct Player {
	// camera
	PerspectiveCamera* camera;
	
	// map of keys corresponding to certain controls
	Keymap keymap;
	
	// last bounding box the player was standing on
	BoundingBox* currentBbox;
	
	// if the player is currently locked to an axis (by the edge of a bbox) then this is set to the axis, otherwise it's set to 0, 0
	glm::vec2 lockAxis;
	
	// mouse sensitivity
	// FIXME: not sure where else to put this, but putting it here goes against the precedent set by other settings
	float mouseSensitivity;
};

// scene
// contain static renderable objects
struct Scene {
	// window
	Window* window;
	
	std::string* gameDir;
	
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
	
	// the rate at which updatePlayerPosition should approach the desired height
	// the formula for the player height given a desired height is:
	// currentHeight + (desiredHeight - currentHeight) * min(heightSpeed*delta, 1)
	// note that the delta correction isn't perfect and results in slightly faster speeds as framerate increases, and is only noticeable wrong at really low framerates (less than 5 fps).  At reasonable height speeds and reasonable frame rates, the difference is negligible (<0.5 seconds difference to reach desired height)
	// the delta correction results in heightSpeed's units being approximately percentage / second, so I advise taking that into consideration when aiming for a good height speed
	float heightSpeed;
};


// struct containing a single parameter, either float or string
// use isFloat to determine which value to access from data
// TODO: convert to union or type casts?
struct Parameter {
	bool isFloat;
	
	// data
	std::string str;
	float fl;
	
	uint32_t index;
};

// struct containing string/number parameters as well as sub-parameters
struct Parameters {
	std::vector<Parameter> parameters;
	
	std::vector<Parameters*> subparameters;
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
	
	// parsing subparameters or not (then it needs to ignore comma)
	bool parsingSubparameters;
};

// methods //

// trigger management
bool onStartChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);
bool onEnterChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);
bool onEnterRepeatChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);
bool onExitChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);
bool onKeyPressChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);
bool onKeyHoldChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);
bool onKeyReleaseChecker(Scene* scene, TriggerInfo* triggerInfo, bool inBoundingCube);

void logToConsole(Scene* scene, TriggerInfo* triggerInfo);
void changeSetting(Scene* scene, TriggerInfo* triggerInfo);
void playBackgroundMusicAction(Scene* scene, TriggerInfo* triggerInfo);
void setBackgroundMusicSettings(Scene* scene, TriggerInfo* triggerInfo);
void playAudio(Scene* scene, TriggerInfo* triggerInfo);

TriggerInfo* createTriggerInfo(glm::vec3 position, glm::vec3 scale, std::vector<std::string>* eventStrings, std::vector<float>* eventNumbers, std::vector<std::string>* strings, std::vector<float>* numbers, std::string action);

// parameter management
Parameter createParameter(float fl, uint32_t index);
Parameter createParameter(std::string str, uint32_t index);

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
bool bboxContains(BoundingBox* box, glm::vec2 point);
bool lineIntersectingBbox(glm::vec2 p1, glm::vec2 p2, BoundingBox* bbox);

Scene* createScene(Window* window, std::string gameDir);
void parseWorldIntoScene(Scene* scene, const char* file);
Scene* parseWorld(const char* file, Window* window, std::string gameDir);
bool hasWalkmap(Scene* scene);
void checkTriggers(Scene* scene, Player* player);

#endif