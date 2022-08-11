// texture management

#include <texture.h>
#include <utils.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// load a texture
TextureData* createTextureData(const char* texturePath){
	TextureData* textureData = allocateMemoryForType<TextureData>();
	
	stbi_set_flip_vertically_on_load(true); // flip because opengl expects textures to start at end of buffer
	
	uint8_t *data = stbi_load(texturePath, &textureData->width, &textureData->height, &textureData->channels, 0);
	
	if(data){
		// create texture
		glGenTextures(1, &textureData->texture);
		
		// bind texture
		glBindTexture(GL_TEXTURE_2D, textureData->texture); // bind texture so function calls affect it
		
		// assign parameters
		// TODO: custom texture params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// generate texture
		if(textureData->channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureData->width, textureData->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(textureData->channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData->width, textureData->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		// generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
	} else {
		printf("error loading texture %s\n", texturePath);
		
		free(textureData);
		
		textureData = NULL;
	}
	
	stbi_image_free(data);
	
	return textureData;
}

TextureData* createTextureDataRawCompressed(unsigned char* buffer, uint32_t length){
	TextureData* textureData = allocateMemoryForType<TextureData>();
	
	stbi_set_flip_vertically_on_load(true); // flip because opengl expects textures to start at end of buffer
	
	uint8_t *data = stbi_load_from_memory((const unsigned char*)buffer, length, &textureData->width, &textureData->height, &textureData->channels, 0);
	
	if(data){
		// create texture
		glGenTextures(1, &textureData->texture);
		
		// bind texture
		glBindTexture(GL_TEXTURE_2D, textureData->texture); // bind texture so function calls affect it
		
		// assign parameters
		// TODO: custom texture params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// generate texture
		if(textureData->channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureData->width, textureData->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(textureData->channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData->width, textureData->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		// generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
	} else {
		printf("error loading raw texture\n");
		
		free(textureData);
		
		textureData = NULL;
	}
	
	stbi_image_free(data);
	
	return textureData;
}