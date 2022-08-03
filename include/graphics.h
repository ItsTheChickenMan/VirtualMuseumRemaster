#ifndef VMR_GRAPHICS_H
#define VMR_GRAPHICS_H

// includes //
#include <cstdio>
#include <cstdint>

// glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <camera.h>
#include <utils.h>
#include <shader.h>

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
};

// vertex data
struct VertexData {
	uint32_t vbo; // vertex buffer object
	uint32_t vao; // vertex array object
	
	uint32_t vertexCount; // number of vertices
	uint32_t sizeInBytes;
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

// vertex data management
VertexData* createVertexData(float *vertices, uint32_t vertexCount, uint32_t sizeInBytes, uint32_t *componentOrder, uint32_t numComponents);
void renderVertexData(VertexData* data);

// renderable object management
RenderableObject* createRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
void setRenderableObjectTransform(RenderableObject* object, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
void renderRenderableObject(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx);

void printMat4(glm::mat4 matrix);

#endif