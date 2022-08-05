// remaster of the virtual museum I made for history about a year ago

#include <graphics.h>
#include <shader.h>
#include <camera.h>
#include <utils.h>
#include <shapes.h>
#include <audio.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>

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
	uint32_t screenHeight = 720;
	
	Window* window = createWindow(screenWidth, screenHeight, "Virtual Museum Remastered");
	
	// make sure it exists
	if(window == NULL){
		printf("There was an error creating the render window\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Done\nLoading shaders...");
	
	// load shaders
	// NOTE: make sure to only do this when the window is created, so as to have a valid context
	
	// barebones shader
	uint32_t barebonesVs = createShader(GL_VERTEX_SHADER, "./res/shader/barebones/vertex.glsl");
	uint32_t barebonesFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/barebones/fragment.glsl");
	
	uint32_t barebonesShader = createShaderProgram(barebonesVs, barebonesFs, true);
	
	// renderable object test
	uint32_t renderableObjectTestVs = createShader(GL_VERTEX_SHADER, "./res/shader/renderableObjectTest/vertex.glsl");
	uint32_t renderableObjectTestFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/renderableObjectTest/fragment.glsl");
	
	ShaderProgramEx* renderableObjectTestShader = createShaderProgramEx(renderableObjectTestVs, renderableObjectTestFs, true);
	
	// texture test
	uint32_t textureTestVs = createShader(GL_VERTEX_SHADER, "./res/shader/textureTest/vertex.glsl");
	uint32_t textureTestFs = createShader(GL_FRAGMENT_SHADER, "./res/shader/textureTest/fragment.glsl");
	
	ShaderProgramEx* textureTestShader = createShaderProgramEx(textureTestVs, textureTestFs, true);
	
	// load texture data
	printf("Done\nLoading textures...");
	
	TextureData* defaultTexture = createTextureData("./res/textures/default.png");
	
	// load vertex data
	printf("Done\nLoading vertex data...");
	
	uint32_t vertexOnlyOrder[] = {0};
	uint32_t vertexAndTextureOrder[] = {0, 1};
	uint32_t defaultComponentOrder[] = {0, 1, 2};
	
	VertexData* cubeData = createVertexData(cube, 36, sizeof(cube), defaultComponentOrder, sizeof(defaultComponentOrder)/sizeof(uint32_t));
	VertexData* triangle2DData = createVertexData(triangle_2D_Tex, 3, sizeof(triangle_2D_Tex), vertexAndTextureOrder, sizeof(vertexAndTextureOrder)/sizeof(uint32_t));
	
	// load sounds
	printf("Done\nLoading sounds...");
	
	std::string filenames[] = {"./res/sounds/test1.ogg", "./res/sounds/test2.ogg"};
	std::string keys[] = {"test1", "test2"};
	
	loadSoundFileBatch(filenames, keys, sizeof(filenames)/sizeof(std::string));
	
	// load scene things (camera, renderable objects)
	printf("Done\nLoading scene...");
	
	// create camera
	PerspectiveCamera* camera = createPerspectiveCamera(glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::radians(45.f), (float)screenWidth, (float)screenHeight, 0.1f, 100.f);

	// create scene objects
	RenderableObject* object = createRenderableObject(triangle2DData, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f), glm::vec3(1.0f));
	
	printf("Done\nRender Loop Starting\n");
	
	// render loop //
	while(!shouldWindowClose(window)){
		// update camera
		//updateCameraViewMatrix(camera, glm::vec3(sin(glfwGetTime()*1.25f)*5.f, 0.0f, 0.0f), glm::vec3(0.0f));
		
		// update cube position
		//setRenderableObjectTransform(object, glm::vec3(sin(glfwGetTime()/4)*5.f, 0.0f, cos(glfwGetTime()/4)*5.f), glm::vec3(0.0f, -glfwGetTime()*10, 0.0f), glm::vec3(1.0f));
		
		// sounds //
		updateSounds();
		
		// render calls //
		clearWindow(0.3f, 0.0f, 0.0f);
	
		// bind shader
		useProgramEx(textureTestShader);
		
		// bind textures
		setProgramExUniformTexture(textureTestShader, "texture1", defaultTexture);
		
		// render
		renderRenderableObject(object, camera, textureTestShader);
		//renderVertexData(triangle2DData);
		
		// reset textures
		resetProgramExUniformTextures(textureTestShader);
		
		// swap buffers
		updateWindow(window);
	}
	
	// free allocated memory
	free(object);
	
	free(triangle2DData);
	free(cubeData);
	
	// kill graphics
	terminateGraphics();
	
	free(window);
	
	return EXIT_SUCCESS;
}