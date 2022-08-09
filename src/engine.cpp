// museum engine manager

#include <engine.h>

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
void renderScene(Scene* scene, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// loop through vertex data
	for (std::map<VertexData*, std::vector<TexturedRenderableObject*>*>::iterator it = scene->staticObjects->begin(); it != scene->staticObjects->end(); it++){
    // bind vertex data
		bindVertexData(it->first);
		
		if(!it->second) continue;
		
		
		// render each object
		for(uint32_t i = 0; i < it->second->size(); i++){
			TexturedRenderableObject* object = it->second->at(i);
			
			if(object) renderTexturedRenderableObjectNoBind(object, camera, programEx);
		}
		
		// unbind vao
		glBindVertexArray(0);
	}
}