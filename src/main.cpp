// remaster of the virtual museum I made for history about a year ago

#include <engine.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <world.h>

#include <glm/gtx/norm.hpp>

glm::vec3 calculateMovementVector(Window* window, PerspectiveCamera* camera);
glm::vec3 calculateRotationVector();

int main(){
	// init //
	
	// set stdout buffer flushed without buffering, because line buffering doesn't work in msys2 terminal
	#ifdef __MINGW64__
		setvbuf(stdout, NULL, _IONBF, 0);
	#endif
	
	// initialize graphics
	if(initGraphics() != SUCCESS){
		printf("There was an error initializing graphics\n");
		exit(EXIT_FAILURE);
	}
	
	// create render window
	printf("Creating window...");
	
	uint32_t screenWidth = 1280;
	//uint32_t screenWidth = 800;
	
	uint32_t screenHeight = 720;
	//uint32_t screenHeight = 600;
	
	Window* window = createWindow(screenWidth, screenHeight, "Virtual Museum Remastered");
	
	// make sure it exists
	if(window == NULL){
		printf("\nThere was an error creating the render window\n");
		exit(EXIT_FAILURE);
	}
	
	initMouseManager(window);
	
	printf("Done\nLoading shaders...");
	
	// load shaders
	// NOTE: make sure to only do this when the window is created, so as to have a valid context
	
	// barebones shader
	//uint32_t barebonesVs = createShader(GL_VERTEX_SHADER, "./res/shader/barebones/vertex.glsl");
	//uint32_t barebonesFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/barebones/fragment.glsl");
	
	//uint32_t barebonesShader = createShaderProgram(barebonesVs, barebonesFs, true);
	
	// renderable object test
	//uint32_t renderableObjectTestVs = createShader(GL_VERTEX_SHADER, "./res/shader/renderableObjectTest/vertex.glsl");
	//uint32_t renderableObjectTestFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/renderableObjectTest/fragment.glsl");
	
	//ShaderProgramEx* renderableObjectTestShader = createShaderProgramEx(renderableObjectTestVs, renderableObjectTestFs, true);
	
	// texture test
	//uint32_t textureTestVs = createShader(GL_VERTEX_SHADER, "./res/shader/textureTest/vertex.glsl");
	//uint32_t textureTestFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/textureTest/fragment.glsl");
	
	//ShaderProgramEx* textureTestShader = createShaderProgramEx(textureTestVs, textureTestFs, true);
	
	// lighting shader
	uint32_t lightingVs = createShader(GL_VERTEX_SHADER, "./res/shader/lighting/vertex.glsl");
	uint32_t lightingFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/lighting/fragment.glsl");
	
	ShaderProgramEx* lightingShader = createShaderProgramEx(lightingVs, lightingFs, true);
	
	// load sounds
	printf("Done\nLoading sounds...");
	
	std::string filenames[] = {"./res/sounds/test1.ogg", "./res/sounds/test2.ogg"};
	std::string keys[] = {"test1", "test2"};
	
	loadSoundFileBatch(filenames, keys, sizeof(filenames)/sizeof(std::string));
	
	// load scene things (camera, renderable objects)
	printf("Done\nLoading scene...");
	
	// create camera
	PerspectiveCamera* camera = createPerspectiveCamera(glm::vec3(-2.0f, 2.5f, 0.0f), glm::vec3(0.0f), glm::radians(45.f), (float)screenWidth, (float)screenHeight, 0.1f, 100.f);

	// create scene objects
	//RenderableObject* object = createRenderableObject(cubeData, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f), glm::vec3(1.0f));
	
	Scene* scene = parseWorld("./res/worlds/basic.world");
	
	printf("Done\nRender Loop Starting\n");
	
	// render loop //
	while(!shouldWindowClose(window)){
		// update camera
		glm::vec3 rotationVector = calculateRotationVector();
		glm::vec3 movementVector = calculateMovementVector(window, camera);
		
		rotateCamera(camera, rotationVector);
		constrainCameraRotation(camera, glm::vec3(glm::radians(-89.f), NO_LB, NO_LB), glm::vec3(glm::radians(89.f), NO_UB, NO_UB));
		translateCamera(camera, movementVector);
		
		resetMouseDelta();
		
		// sounds //
		updateSounds();
		
		// render calls //
		clearWindow(0.3f, 0.0f, 0.0f);
	
		// bind shader
		useProgramEx(lightingShader);
		
		// render
		renderScene(scene, camera, lightingShader);
		
		// swap buffers
		updateWindow(window);
	}
	
	// kill graphics
	terminateGraphics();
	
	free(window);
	
	return EXIT_SUCCESS;
}

glm::vec3 calculateMovementVector(Window* window, PerspectiveCamera* camera){
	// settings
	int32_t keymap[] = {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT};
	
	glm::vec3 side = glm::normalize(glm::cross(camera->forward, camera->up));
	
	glm::vec3 directions[] = {camera->forward, side * -1.f, camera->forward * -1.f, side, camera->up, camera->up * -1.f};
	
	float speed = 0.1f;
	
	if(glfwGetKey(window->glfwWindow, GLFW_KEY_E) == GLFW_PRESS){
		speed /= 3.0f;
	}
	
	if(glfwGetKey(window->glfwWindow, GLFW_KEY_Q) == GLFW_PRESS){
		speed *= 3.0f;
	}
	
	// calculation
	
	glm::vec3 movementVector = glm::vec3(0);
	
	
	for(uint32_t i = 0; i < (uint32_t)sizeof(keymap)/sizeof(int32_t); i++){
		int32_t key = keymap[i];
		
		if(glfwGetKey(window->glfwWindow, key) == GLFW_PRESS){
			movementVector += directions[i];
		}
	}
	
	if(glm::length2(movementVector) != 0.0f) movementVector = glm::normalize(movementVector) * speed;
	
	movementVector.y = 0;
	
	return movementVector;
}

glm::vec3 calculateRotationVector(){
	float sensitivity = 0.004f;
	
	glm::vec2 delta = getMouseDelta();
	
	delta *= sensitivity;
	
	return glm::vec3(-delta.y, delta.x, 0);
}