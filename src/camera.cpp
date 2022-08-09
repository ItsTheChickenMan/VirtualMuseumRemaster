// camera manager

#include <camera.h>
#include <utils.h>

#include <cmath>
#include <algorithm>

// create perspective camera
PerspectiveCamera *createPerspectiveCamera(glm::vec3 position, glm::vec3 rotation, float fov, float screenWidth, float screenHeight, float near, float far){
	PerspectiveCamera *camera = allocateMemoryForType<PerspectiveCamera>(); 
	
	camera->up = glm::vec3(0, 1, 0);
	
	updateCameraProjectionMatrix(camera, fov, screenWidth, screenHeight, near, far);
	updateCameraViewMatrix(camera, position, rotation);
	
	return camera;
}

// completely updates perspective camera's projection matrix with new fov, aspect, and clipping values
void updateCameraProjectionMatrix(PerspectiveCamera *camera, float fov, float screenWidth, float screenHeight, float near, float far){
	// stores values
	camera->fov = fov;
	camera->screenWidth = screenWidth;
	camera->screenHeight = screenHeight;
	camera->near = near;
	camera->far = far;
	
	// update projection matrix
	camera->projection = glm::perspective(fov, screenWidth/screenHeight, near, far);
}

// completely updates perspective camera's view matrix with new position and rotation values.
void updateCameraViewMatrix(PerspectiveCamera *camera){
	// calculate forward
	camera->forward = glm::normalize(glm::vec3(
		cos(camera->rotation.y) * cos(camera->rotation.x),
		sin(camera->rotation.x),
		sin(camera->rotation.y) * cos(camera->rotation.x)
	));
	
	camera->view = glm::lookAt(camera->position, camera->position+camera->forward, camera->up);
	
	camera->pv = camera->projection * camera->view;
}

// completely updates perspective camera's view matrix with new position and rotation values.
void updateCameraViewMatrix(PerspectiveCamera *camera, glm::vec3 position, glm::vec3 rotation){
	camera->position = position;
	camera->rotation = rotation;
	
	updateCameraViewMatrix(camera);
}

// adds some translation to camera
void translateCamera(PerspectiveCamera *camera, glm::vec3 translation){
	camera->position += translation;
	
	updateCameraViewMatrix(camera);
}

// adds some rotation to camera
void rotateCamera(PerspectiveCamera *camera, glm::vec3 rotation){
	camera->rotation += rotation;
	
	updateCameraViewMatrix(camera);
}

void constrainCameraRotation(PerspectiveCamera *camera, glm::vec3 lowerBounds, glm::vec3 upperBounds){
	for(uint32_t i = 0; i < 3; i++){
		float lowerBound = lowerBounds[i];
		float upperBound = upperBounds[i];
		
		camera->rotation[i] = std::clamp(camera->rotation[i], lowerBound, upperBound);
	}
}