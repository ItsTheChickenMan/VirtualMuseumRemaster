#ifndef VMR_TEXTURE_H
#define VMR_TEXTURE_H

// includes
#include <cstdint>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

/**
	*	@brief Struct representing texture data
	*
	*	Struct representing texture data, containing its width, height, # of channels and the texture name which OpenGL uses for rendering
*/
struct TextureData {
	int32_t width;
	int32_t height;
	int32_t channels;
	
	uint32_t texture; /**< @brief OpenGL texture name used in rendering */
};

/**
	*	@brief Create texture data
	*
	*	Load an image at texturePath and create texture data from it.
	*
	*	stb_image seems to have trouble when loading png images with only 3 channels, so make sure bit depth is 32 with PNGs.
	*
	*	@param texturePath path to texture to load
	*	@return TextureData to use in rendering
*/
TextureData* createTextureData(const char* texturePath);

// create texture data from raw compressed image
TextureData* createTextureDataRawCompressed(unsigned char* buffer, uint32_t length);

#endif