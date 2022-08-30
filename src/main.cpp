// remaster of the virtual museum I made for history about a year ago

#include <engine.h>
#include <world.h>

#include <stdexcept>
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <argparse/argparse.hpp>

#include <glm/gtx/norm.hpp>

#define PROG_NAME "world"
#define PROG_VERSION "0.1-dev"


void initializeArguments(argparse::ArgumentParser& parser);

glm::vec3 calculateMovementVector(Window* window, PerspectiveCamera* camera);
glm::vec3 calculateRotationVector();

int main(int argc, char** argv){
	// init //
	
	// set stdout buffer flushed without buffering, because line buffering doesn't work in msys2 terminal
	#ifdef __MINGW64__
		setvbuf(stdout, NULL, _IONBF, 0);
	#endif
	
	// parse arguments
	argparse::ArgumentParser argParser = argparse::ArgumentParser(PROG_NAME, PROG_VERSION);
	
	initializeArguments(argParser);
	
	try {
		argParser.parse_args(argc, argv);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << argParser; // NOTE: logs help message
		
		exit(EXIT_FAILURE);
	}
	
	// get game directory
	std::string gameDir = argParser.get<std::string>("--game-dir");
	
	// initialize graphics
	if(initGraphics() != SUCCESS){
		printf("There was an error initializing graphics\n");
		exit(EXIT_FAILURE);
	}
	
	// create render window
	printf("Creating window...");
	
	uint32_t screenWidth = argParser.get<uint32_t>("-w");
	uint32_t screenHeight = argParser.get<uint32_t>("-h");
	
	std::string title = argParser.get<std::string>("-t");
	
	Window* window = createWindow(screenWidth, screenHeight, title.c_str());
	
	// make sure it exists
	if(window == NULL){
		printf("\nThere was an error creating the render window\n");
		exit(EXIT_FAILURE);
	}
	
	float xSensitivity = 0.5f;
	float ySensitivity = 0.5f;
	
	initMouseManager(window, xSensitivity, ySensitivity);
	
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
	uint32_t lightingVs = createShader(GL_VERTEX_SHADER, "./shader/lighting/vertex.glsl");
	uint32_t lightingFs = createShader(GL_FRAGMENT_SHADER, "./shader/lighting/fragment.glsl");
	
	ShaderProgramEx* lightingShader = createShaderProgramEx(lightingVs, lightingFs, true);
	
	// load scene things (camera, renderable objects)
	printf("Done\nLoading scene...");
	
	// create camera
	// -6.594845, 5.227420, -4.362258, -0.564000, 0.656000, 0.000000
	// 8.823283, 7.303828, 8.691005, -0.615999, 3.887995, 0.000000
	PerspectiveCamera* camera = createPerspectiveCamera(glm::vec3(0), glm::vec3(0), glm::radians(45.f), (float)screenWidth, (float)screenHeight, 0.1f, 100.f);
	
	// create player
	Keymap keymap = (Keymap){ GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_E, GLFW_KEY_Q };
	
	Player* player = createPlayer(camera, keymap);
	
	// create scene
	Scene* scene = createScene(window, player, argParser.get<std::string>("--game-dir"));
	
	// load any world/walkmap files from arguments
	std::vector<std::string> worldPaths = argParser.get<std::vector<std::string>>("--world");
	std::vector<std::string> walkmapPaths = argParser.get<std::vector<std::string>>("--walkmap");
	
	for(uint32_t i = 0; i < worldPaths.size(); i++){
		parseWorldIntoScene(scene, (*scene->gameDir + "/worlds/" + worldPaths[i]).c_str());
	}
	
	for(uint32_t i = 0; i < walkmapPaths.size(); i++){
		parseWorldIntoScene(scene, (*scene->gameDir + "/worlds/" + walkmapPaths[i]).c_str());
	}
	
	/*for(uint32_t i = 0; i < scene->pointLights->size(); i++){
		printf("%f, %f, %f\n", scene->pointLights->at(i)->color.x, scene->pointLights->at(i)->color.y, scene->pointLights->at(i)->color.z);
	}*/
	
	printf("Done\nRender Loop Starting\n");
	
	// render loop //
	double delta = 0.0;
	double lastFrame = glfwGetTime();
	while(!shouldWindowClose(window)){
		// update delta
		double time = glfwGetTime();
		delta = time-lastFrame;
		lastFrame = time;
		
		// update camera
		glm::vec3 rotationVector = calculateRotationVector();
		//glm::vec3 movementVector = calculateMovementVector(window, camera);
		
		// update player position (works regardless of walkmap presence)
		updatePlayerPosition(player, scene, window, delta);
		
		rotateCamera(camera, rotationVector);
		constrainCameraRotation(camera, glm::vec3(glm::radians(-89.f), NO_LB, NO_LB), glm::vec3(glm::radians(89.f), NO_UB, NO_UB));
		//translateCamera(camera, movementVector);
		
		// update sound listener position
		updateListener(player->camera->position, player->camera->forward);
		
		// check triggers
		checkTriggers(scene);
		
		//printf("fr: %f\n", 1.0/delta);
		//printf("camera: %f, %f, %f, %f, %f, %f\n", camera->position.x, camera->position.y, camera->position.z, camera->rotation.x, camera->rotation.y, camera->rotation.z);
		
		// reset mouse
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

// set up argument parser
void initializeArguments(argparse::ArgumentParser& parser){
	// description
	parser.add_description("Loads a game into the world engine.");
	
	// game related args
	parser.add_argument("--game-dir")
		.help("directory to the game's files.")
		.required();
		
	parser.add_argument("--world")
		.help("path to a .world file to load, relative to the worlds folder in the game dir.  this can be used multiple times to load multiple .world files.")
		.default_value<std::vector<std::string>>({})
		.append();
		
	parser.add_argument("--walkmap")
		.help("path to a .walkmap file to load, relative to the worlds folder in the game dir.  this can be used multiple times to load multiple .walkmap files.")
		.default_value<std::vector<std::string>>({})
		.append();
	
	// window related args
	parser.add_argument("-t", "--title")
		.help("title of game window.")
		.default_value("World Engine");
		
	parser.add_argument("-w", "--width")
		.help("pixel width of game window.")
		.default_value<uint32_t>(1280)
		.scan<'i', uint32_t>();
		
	parser.add_argument("-h", "--height")
		.help("pixel height of game window.")
		.default_value<uint32_t>(720)
		.scan<'i', uint32_t>();
}

glm::vec3 calculateMovementVector(Window* window, PerspectiveCamera* camera){
	// settings
	int32_t keymap[] = {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT};
	
	glm::vec3 side = glm::normalize(glm::cross(camera->forward, camera->up));
	
	glm::vec3 directions[] = {camera->forward, side * -1.f, camera->forward * -1.f, side, camera->up, camera->up * -1.f};
	
	float speed = 0.05f;
	
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