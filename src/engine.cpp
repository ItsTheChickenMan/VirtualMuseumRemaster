// museum engine manager

#include <engine.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

// assumes a texture uniform "texture1" exists in the shader
void renderTexturedRenderableObject(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// null checks
	if(!texturedRenderableObject || !camera || !programEx) return;
	
	// bind texture
	TextureData* textureData = texturedRenderableObject->textureData;
	
	// FIXME: no render for no textures?
	if(!textureData) return;
	
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
	
	// FIXME: no render for no textures?
	if(!textureData) return;
	
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
	// add lights to shader
	for(uint32_t i = 0; i < scene->pointLights->size(); i++){
		PointLight* light = scene->pointLights->at(i);
		
		if(light != NULL) addProgramExPointLight(programEx, "pointLights", light);
	}
	
	// loop through vertex data
	for (std::map<VertexData*, std::vector<TexturedRenderableObject*>*>::iterator it = scene->staticObjects->begin(); it != scene->staticObjects->end(); it++){
    // bind vertex data
		bindVertexData(it->first);
		
		if(!it->second) continue;
		
		// render each object
		for(uint32_t i = 0; i < it->second->size(); i++){
			TexturedRenderableObject* object = it->second->at(i);
			
			if(!object) continue;
			
			// set model matrix (necessary for lighting)
			glUniformMatrix4fv(getProgramExUniformLocation(programEx, "model"), 1, GL_FALSE, glm::value_ptr(object->renderableObject->modelMatrix));
			
			// set normal matrix (necessary for lighting)
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(object->renderableObject->modelMatrix)));
			glUniformMatrix3fv(getProgramExUniformLocation(programEx, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
			renderTexturedRenderableObjectNoBind(object, camera, programEx);
		}
		
		// unbind vao
		glBindVertexArray(0);
	}
	
	// reset lights
	resetProgramExPointLights(programEx);
}