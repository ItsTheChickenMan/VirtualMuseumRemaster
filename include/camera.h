// camera manager

#ifndef VHR_CAMERA_H
#define VHR_CAMERA_H

// includes //
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// structs //

// camera
struct PerspectiveCamera {
	glm::vec3 forward; // forward vector (where we're looking)
	glm::vec3 up; // up vector (which way is up)
	
	glm::vec3 position; // position of the camera
	glm::vec3 rotation; // rotation of the camera
	
	float fov; // field of vision
	float screenWidth; // width of screen
	float screenHeight; // height of screen
	float near; // near plane limit
	float far; // far plane limit
	
	glm::mat4 projection; // projection matrix (generally calculated once and left as is)
	glm::mat4 view; // view matrix (generally set once per frame)
	glm::mat4 pv; // combination of projection and view matrix (generally should be calculated once per frame for combination with model matrices)
};

// camera management
PerspectiveCamera* createPerspectiveCamera(glm::vec3 position, glm::vec3 rotation, float fov, float screenWidth, float screenHeight, float near, float far);
void updateCameraProjectionMatrix(PerspectiveCamera *camera, float fov, float screenWidth, float screenHeight, float near, float far);

void updateCameraViewMatrix(PerspectiveCamera *camera);
void updateCameraViewMatrix(PerspectiveCamera *camera, glm::vec3 position, glm::vec3 rotation);
void translateCamera(PerspectiveCamera *camera, glm::vec3 translation);
void rotateCamera(PerspectiveCamera *camera, glm::vec3 rotation);

#endif