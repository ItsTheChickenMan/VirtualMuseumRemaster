// shader management

#ifndef VHR_SHADER_H
#define VHR_SHADER_H

// includes //
#include <glad/glad.h>
#include <texture.h>

#include <map>
#include <string>

// macros //
#define NULL_SHADER 0
#define NULL_SHADER_PROGRAM 0

// structs //

// shader program with extended uniform and texture management
struct ShaderProgramEx {
	GLuint program;
	
	// uniform management
	std::map<std::string, GLint>* uniforms;
	
	// texture management
	uint32_t textureUnits; // number of currently bound texure units
	int32_t maxTextureUnits; // maximum supported texture units (implementation dependent)
};

// method definitions //
GLuint createShader(GLenum shaderType, const char* source);
GLchar* getShaderInfoLog(GLuint shader);

GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader, bool deleteShaders);
GLchar* getShaderProgramInfoLog(GLuint shaderProgram);

ShaderProgramEx* createShaderProgramEx(GLuint vertexShader, GLuint fragmentShader, bool deleteShaders);
void loadShaderProgramExUniformLocations(ShaderProgramEx* programEx);
void useProgramEx(ShaderProgramEx* programEx);
GLint getProgramExUniformLocation(ShaderProgramEx* programEx, std::string name);
void setProgramExUniformTexture(ShaderProgramEx *programEx, const char* location, TextureData *textureData);
void resetProgramExUniformTextures(ShaderProgramEx *programEx);

#endif