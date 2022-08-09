// world parser

#ifndef VMR_WORLD_H
#define VMR_WORLD_H

// includes //
#include <graphics.h>
#include <texture.h>
#include <lighting.h>

#include <string>
#include <vector>

// structs //

// textured renderable object
struct TexturedRenderableObject {
	TextureData* textureData;
	
	RenderableObject* renderableObject;
};

// scene
// contain static renderable objects
struct Scene {
	// loaded vertex data
	std::map<std::string, VertexData*>* vertexData;
	
	// loaded textures
	std::map<std::string, TextureData*>* textures;
	
	// objects
	std::map<VertexData*, std::vector<TexturedRenderableObject*>*>* staticObjects;
	
	// lights
	std::vector<PointLight*>* pointLights;
};

// world blocks (hold information about blocks)

// texture block
struct TextureBlock {
	const static uint32_t numStrings = 2;
	
	// vector of string parameters
	std::vector<std::string>* strings;
	
	// index of string parameter being written to
	uint32_t parameterIndex;
	
	// string buffer for parameter
	std::string* parameterBuffer;
};

// vertex data block
struct VertexDataBlock {
	const static uint32_t numStrings = 2;
	
	// vector of string parameters
	std::vector<std::string>* strings;
	
	// index of string parameter being written to
	uint32_t parameterIndex;
	
	// string buffer for parameter
	std::string* parameterBuffer;
};

// object block
struct ObjectBlock {
	const static uint32_t numFloats = 9;
	const static uint32_t numStrings = 2;
	
	// vector of float parameters
	std::vector<float>* floats;
	
	// vector of string parameters
	std::vector<std::string>* strings;
	
	// index of parameter being written to
	uint32_t parameterIndex;
	
	// string buffer for parameter
	std::string* parameterBuffer;
};

struct LightBlock {
	const static uint32_t numFloats = 11;
	
	// vector of float parameters
	std::vector<float>* floats;
	
	// index of parameter being written to
	uint32_t parameterIndex;
	
	// string buffer for parameter
	std::string* parameterBuffer;
};

// methods //
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, TextureData* texture);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, TextureData* texture);
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, const char* texturePath);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const char* texturePath);

Scene* createScene();
Scene* parseWorld(const char* file);

#endif