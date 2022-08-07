// mouse management
#include <mouse.h>

// mouse position and delta
glm::vec2 position;
glm::vec2 delta;

void cursorPositionUpdateCallback(GLFWwindow* window, double xpos, double ypos){
	// update delta
	glm::vec2 temp = glm::vec2(xpos, ypos);
	delta = temp - position;
	
	// update position
	position = temp;
}

void initMouseManager(Window* window){
	glfwSetInputMode(window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// get raw input if available
	if (glfwRawMouseMotionSupported()){
    glfwSetInputMode(window->glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	} else {
		printf("\nRaw mouse motion is not supported\n");
	}
	
	glfwSetCursorPosCallback(window->glfwWindow, cursorPositionUpdateCallback);
	
	position = glm::vec2(0);
	delta = glm::vec2(0);
}

// call after all usage of getMouseDelta 
void resetMouseDelta(){
	delta = glm::vec2(0);
}

glm::vec2 getMousePosition(){
	return position;
}

glm::vec2 getMouseDelta(){
	return delta;
}