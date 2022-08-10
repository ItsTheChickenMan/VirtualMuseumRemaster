// graphics manager/window manager

// includes //

#include <graphics.h>
#include <utils.h>
#include <camera.h>
#include <shader.h>

#include <string>

// window management //

// initialize glfw/glad
// should only need to be called once at the start of the program
InitGraphicsStatus initGraphics(){
	// initialize glfw
	if(glfwInit() != GL_TRUE){
		return GLFW_INIT_ERROR;
	}
	
	// set glfw window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opengl version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // core profile
	
	// temporarily make non-resizeable to avoid the render issues that causes
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	return SUCCESS;
}

// kill glfw/other graphics stuff
void terminateGraphics(){
	glfwTerminate();
}

// create a new render window
// returns NULL if an error occurred
Window* createWindow(int32_t width, int32_t height, const char* title){
	// allocate memory for window
	Window* window = allocateMemoryForType<Window>();
	
	// create glfw window
	window->glfwWindow = (GLFWwindow*)glfwCreateWindow(width, height, title, NULL, NULL);
	
	// check to ensure window created properly
	if(!window->glfwWindow){
		terminateGraphics();
		return NULL;
	}
	
	// make window current context
	glfwMakeContextCurrent(window->glfwWindow);
	
	// load gl methods
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		terminateGraphics();
		return NULL;
	}
	
	// set viewport size
	glViewport(0, 0, width, height);
	
	// default settings
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glfwSwapInterval(1);
	
	return window;
}

// if the window has received some kind of close event
bool shouldWindowClose(Window* window){
	return glfwWindowShouldClose(window->glfwWindow);
}

void updateWindow(Window* window){
	// swap buffers
	glfwSwapBuffers(window->glfwWindow);
	
	// poll for events
	glfwPollEvents();
}

// clear the window and replace with a color
// also flushes the depth map
void clearWindow(float r, float g, float b){
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// vertex data //

// create vertex data from a set of vertices
// if vertexCount is not equal to sizeof(vertices)/sizeof(float), there will be problems
// for component order: 
// 0 = 3-component vertices
// 1 = 2-component texture coordinates
// 2 = 3-component normals
VertexData* createVertexData(float *vertices, uint32_t vertexCount, uint32_t sizeInBytes, uint32_t *componentOrder, uint32_t numComponents){
	// allocate space
	VertexData* data = allocateMemoryForType<VertexData>();
	
	// set default values in case any methods below fail
	data->vbo = 0;
	data->vao = 0;
	data->vertexCount = vertexCount;
	data->sizeInBytes = sizeInBytes;
	
	// create vertex buffer object
	glGenBuffers(1, &data->vbo);
	
	// check for success
	// FIXME: actual error checking using gl methods
	if(data->vbo == 0) return NULL;
	
	// bind buffer to array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	
	// copy vertex data to buffer
	glBufferData(GL_ARRAY_BUFFER, data->sizeInBytes, vertices, GL_STATIC_DRAW); // target, size, data, purpose
	
	// generate vertex attribute object
	glGenVertexArrays(1, &data->vao);
	
	// check for success
	if(data->vao == 0) return NULL;
	
	// bind vao
	glBindVertexArray(data->vao);
	
	// total stride for each element
	uint32_t stride = 0;
	
	// calculate total size of stride
	for(uint32_t i = 0; i < numComponents; i++){
		uint32_t component = componentOrder[i];
		uint32_t componentSize = 3 - (component % 2); // this will need to be changed should any more components be supported
		
		stride += componentSize;
	}
	
	// offset of the current component
	uint32_t offset = 0;
	
	// create vertex attribues
	for(uint32_t i = 0; i < numComponents; i++){
		uint32_t component = componentOrder[i];
		uint32_t componentSize = 3 - (component % 2); // this will need to be changed should any more components be supported
		
		glVertexAttribPointer(component, componentSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset*sizeof(float)));
		glEnableVertexAttribArray(component);
		
		offset += componentSize;
	}
	
	// unbind buffer and arary
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return data;
}

// bind vertex data vao
void bindVertexData(VertexData* data){
	glBindVertexArray(data->vao);
}

// bind vertex data and call draw arrays
void renderVertexData(VertexData* data){
	// bind vao
	bindVertexData(data);
	
	// call draw arrays
	renderVertexDataNoBind(data);
	
	// unbind vertex array for neatness
	glBindVertexArray(0);
}

// call draw arrays with no bind
void renderVertexDataNoBind(VertexData* data){
	// call draw arrays
	glDrawArrays(GL_TRIANGLES, 0, data->vertexCount);
}

// create a renderable object (vertex data w/ model matrix)
RenderableObject* createRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){
	// allocate memory
	RenderableObject* object = allocateMemoryForType<RenderableObject>();
	
	// copy values
	object->vertexData = vertexData;
	
	// assign transform
	setRenderableObjectTransform(object, position, rotation, scale);
	
	return object;
}

// assign translation, rotation, and scale values to an object's transform
void setRenderableObjectTransform(RenderableObject* object, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){
	// store values
	object->position = position;
	object->rotation = rotation;
	object->scale = scale;
	
	// translate
	object->modelMatrix = glm::mat4(1.0f);
	
	object->modelMatrix = glm::translate(object->modelMatrix, position);
	
	// rotate around each axis
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.z, glm::vec3(0.f, 0.f, 1.f));
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.y, glm::vec3(0.f, 1.f, 0.f));
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.x, glm::vec3(1.f, 0.f, 0.f));
	
	// scale
	object->modelMatrix = glm::scale(object->modelMatrix, object->scale);
}

// render a renderable object with respect to a perspective camera's transform
// this method assumes that the shader has a pvm uniform, and has already been bound (this method will not call glUseProgram)
void renderRenderableObject(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// create pvm
	glm::mat4 pvm = camera->pv * object->modelMatrix;
	
	// assign uniforms
	glUniformMatrix4fv(getProgramExUniformLocation(programEx, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
	
	// render vertex data
	renderVertexData(object->vertexData);
}

// render a renderable object with respect to a perspective camera's transform
// this method assumes that the shader has a pvm uniform, and has already been bound (this method will not call glUseProgram)
// calls renderVertexDataNoBind
void renderRenderableObjectNoBind(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// create pvm
	glm::mat4 pvm = camera->pv * object->modelMatrix;
	
	// assign uniforms
	glUniformMatrix4fv(getProgramExUniformLocation(programEx, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
	
	// render vertex data
	renderVertexDataNoBind(object->vertexData);
}

// print the contents of a mat4
void printMat4(glm::mat4 matrix){
	for(uint32_t i = 0; i < 4; i++){
		for(uint32_t j = 0; j < 4; j++){
			printf("%f, ", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}