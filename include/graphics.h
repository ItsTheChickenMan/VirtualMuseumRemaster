#ifndef VMR_GRAPHICS_H
#define VMR_GRAPHICS_H

// includes //
#include <cstdio>
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <camera.h>
#include <shader.h>
#include <texture.h>

#include <vector>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

// enums //

// initGraphics() status
typedef enum {
	SUCCESS, // success
	GLFW_INIT_ERROR // glfwInit() returned false
} InitGraphicsStatus;

// structs //

// window
struct Window {
	GLFWwindow* glfwWindow;
	
	int32_t width;
	int32_t height;
};

// vertex
struct Vertex {
	glm::vec3 position;
	glm::vec2 textureCoordinates;
	glm::vec3 normal;
};

// vertex data
struct VertexData {
	uint32_t vbo; // vertex buffer object
	uint32_t vao; // vertex array object
	uint32_t ebo; // elements buffer object
	
	uint32_t vertexCount; // number of vertices
	uint32_t indexCount; // number of indices, if ebo != 0 (otherwise 0)
	uint32_t sizeInBytes;
};

// mesh
struct Mesh {
	VertexData* vertexData;
	
	TextureData* texture; // TODO: multiple?
	glm::vec3 color; // color if texture is null
};

// model
struct Model {
	// meshes
	std::vector<Mesh*>* meshes;
	
	// loaded textures
	std::map<std::string, TextureData*>* textures;
	
	// path to model
	std::string* path;
};

// renderable object
// basically vertex data but with more data
struct RenderableObject {
	VertexData* vertexData;
	
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	
	glm::mat4 modelMatrix;
};

// window management
InitGraphicsStatus initGraphics();
void terminateGraphics();
Window* createWindow(int32_t width, int32_t height, const char* title);
bool shouldWindowClose(Window* window);
void updateWindow(Window* window);
void clearWindow(float r, float g, float b);

// vertex management
Vertex createVertex(glm::vec3 position, glm::vec2 textureCoordinates, glm::vec3 normal);

// vertex data management
VertexData* createVertexData(float *vertices, uint32_t vertexCount, uint32_t sizeInBytes, uint32_t *componentOrder, uint32_t numComponents);
VertexData* createVertexData(Vertex *vertices, uint32_t vertexCount, uint32_t sizeInBytes);
VertexData* createVertexData(std::vector<Vertex> vertices);
VertexData* createVertexData(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
void bindVertexData(VertexData* data);
void renderVertexData(VertexData* data);
void renderVertexDataNoBind(VertexData* data);

// renderable object management
RenderableObject* createRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
void setRenderableObjectTransform(RenderableObject* object, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
void renderRenderableObject(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx);
void renderRenderableObjectNoBind(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx);

// model management
Mesh* createMesh(VertexData* vertexData, TextureData* texture, glm::vec3 color);
Model* createModel();
void loadModels(std::vector<Model*>* models, std::string paths[], uint32_t numPaths);
Model* loadModel(Assimp::Importer& importer, const std::string& path);
Model* loadModel(const std::string& path);

#endif