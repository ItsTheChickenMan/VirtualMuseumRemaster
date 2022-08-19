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
	glm::vec3 color; // color if texture data is null
	
	RenderableObject* renderableObject;
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
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, TextureData* texture);
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, glm::vec3 color);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, TextureData* texture);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color);
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, const char* texturePath);
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const char* texturePath);

BoundingBox* createBbox(glm::vec2 p, glm::vec2 s);
BoundingBox* createBbox(glm::vec3 p, glm::vec2 s);
BoundingBox* createBbox(BoundingBox* original);

Scene* createScene();
void parseWorldIntoScene(Scene* scene, const char* file);
Scene* parseWorld(const char* file);

#endif