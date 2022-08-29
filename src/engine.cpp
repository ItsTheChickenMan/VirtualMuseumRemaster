// museum engine manager

#include <engine.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>

// assumes a texture uniform "texture1" exists in the shader
void renderTexturedRenderableObject(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// null checks
	if(!texturedRenderableObject || !camera || !programEx) return;
	
	// bind texture
	TextureData* textureData = texturedRenderableObject->textureData;
	
	// set color
	glUniform3fv(getProgramExUniformLocation(programEx, "color"), 1, glm::value_ptr(texturedRenderableObject->color));
	
	// set texture
	setProgramExUniformTexture(programEx, "texture1", textureData);
	
	// render
	RenderableObject* object = texturedRenderableObject->renderableObject;
	
	renderRenderableObject(object, camera, programEx);
	
	// reset textures
	resetProgramExUniformTextures(programEx);
}

void renderTexturedRenderableObjectNoBind(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// null checks
	if(!texturedRenderableObject || !camera || !programEx) return;
	
	// bind texture
	TextureData* textureData = texturedRenderableObject->textureData;
	
	// set color
	glUniform3fv(getProgramExUniformLocation(programEx, "color"), 1, glm::value_ptr(texturedRenderableObject->color));
	
	// set texture
	setProgramExUniformTexture(programEx, "texture1", textureData);
	
	// render
	RenderableObject* object = texturedRenderableObject->renderableObject;
	
	renderRenderableObjectNoBind(object, camera, programEx);
	
	// reset textures
	resetProgramExUniformTextures(programEx);
}

// render an entire scene
// assumes uniforms named pointLights, numPointLights, and normalMatrix exist
void renderScene(Scene* scene, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	float maxDistance2 = camera->far * camera->far;
	
	// add lights to shader
	for(uint32_t i = 0; i < scene->pointLights->size(); i++){
		PointLight* light = scene->pointLights->at(i);
		
		if(light != NULL) addProgramExPointLight(programEx, "pointLights", light);
	}
	
	uint32_t renderCalls = 0;
	
	// loop through vertex data
	for (std::map<VertexData*, std::vector<TexturedRenderableObject*>*>::iterator it = scene->staticObjects->begin(); it != scene->staticObjects->end(); it++){
    // bind vertex data
		bindVertexData(it->first);
		
		if(!it->second) continue;
		
		// render each object
		for(uint32_t i = 0; i < it->second->size(); i++){
			TexturedRenderableObject* object = it->second->at(i);
			
			if(!object || !object->visible) continue;
			
			// compute distance from camera
			
			// not perfect but good enough, basically creates a bounding sphere which can be ineffective for long thin objects
			float longestEdge2 = glm::length2(object->renderableObject->scale);
			
			glm::vec3 difVector = object->renderableObject->position - camera->position;
			float distance2 = glm::length2( difVector );
			
			distance2 -= longestEdge2;
			
			// if distance is greater than view distance, cull
			if(distance2 > maxDistance2) continue;
			
			// determine if object is absolutely behind viewer
			float longestEdge = sqrt(longestEdge2);
			
			glm::vec3 furthestPossiblePoint = object->renderableObject->position + camera->forward*longestEdge;
			
			float angle = glm::dot(camera->forward, glm::normalize(furthestPossiblePoint - camera->position) );
			
			if(angle < 0) continue;
			
			// set model matrix (necessary for lighting)
			glUniformMatrix4fv(getProgramExUniformLocation(programEx, "model"), 1, GL_FALSE, glm::value_ptr(object->renderableObject->modelMatrix));
			
			// set normal matrix (necessary for lighting)
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(object->renderableObject->modelMatrix)));
			glUniformMatrix3fv(getProgramExUniformLocation(programEx, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
			
			renderTexturedRenderableObjectNoBind(object, camera, programEx);
			
			renderCalls++;
		}
		
		// unbind vao
		glBindVertexArray(0);
	}
	
	// reset lights
	resetProgramExPointLights(programEx);
	
	//printf("render calls: %d\n", renderCalls);
}