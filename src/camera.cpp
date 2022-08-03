// camera manager

#include <camera.h>
#include <utils.h>

#include <cmath>

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
void updateCameraViewMatrix(PerspectiveCamera *camera, glm::vec3 position, glm::vec3 rotation){
	// calculate forward
	camera->forward = glm::normalize(glm::vec3(
		cos(rotation.y) * cos(rotation.x),
		sin(rotation.x),
		sin(rotation.y) * cos(rotation.x)
	));
	
	camera->view = glm::lookAt(position, position+camera->forward, camera->up);
	
	camera->pv = camera->projection * camera->view;
	
	camera->position = position;
	camera->rotation = rotation;
}