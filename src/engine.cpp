// museum engine manager

#include <engine.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

// assumes a texture uniform "texture1" exists in the shader
void renderTexturedRenderableObject(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// bind texture
	TextureData* textureData = texturedRenderableObject->textureData;
	
	setProgramExUniformTexture(programEx, "texture1", textureData);
	
	// render
	RenderableObject* object = texturedRenderableObject->renderableObject;
	
	renderRenderableObject(object, camera, programEx);
	
	// reset textures
	resetProgramExUniformTextures(programEx);
}

void renderTexturedRenderableObjectNoBind(TexturedRenderableObject* texturedRenderableObject, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// bind texture
	TextureData* textureData = texturedRenderableObject->textureData;
	
	setProgramExUniformTexture(programEx, "texture1", textureData);
	
	// render
	RenderableObject* object = texturedRenderableObject->renderableObject;
	
	renderRenderableObjectNoBind(object, camera, programEx);
	
	// reset textures
	resetProgramExUniformTextures(programEx);
}

// render an entire scene
// assumes uniforms named pointLights and numPointLights
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
			
			// set model matrix (necessary for lighting)
			glUniformMatrix4fv(getProgramExUniformLocation(programEx, "model"), 1, GL_FALSE, glm::value_ptr(object->renderableObject->modelMatrix));
			
			if(object) renderTexturedRenderableObjectNoBind(object, camera, programEx);
		}
		
		// unbind vao
		glBindVertexArray(0);
	}
	
	// reset lights
	resetProgramExPointLights(programEx);
}