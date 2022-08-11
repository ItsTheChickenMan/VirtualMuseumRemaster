// shader management

#include <shader.h>
#include <utils.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>

GLuint createShader(GLenum shaderType, const char* source){
	// read the contents of the source file
	char* contents = read_entire_file(source);
	
	// check for valid contents
	if(contents == NULL) return NULL_SHADER;
	
	// create shader object
	GLuint shader = glCreateShader(shaderType);
	
	// load shader source
	glShaderSource(shader, 1, &contents, NULL);
	
	// compile
	glCompileShader(shader);
	
	// free contents
	free(contents);
	
	// get compile status and log errors
	GLint shaderCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
	
	// check compile status
	if(!shaderCompiled){
		// get info log
		GLchar* infoLog = getShaderInfoLog(shader);
		
		if(!infoLog){
			printf("There was a shader compilation error, but the info log is empty.\n");
		} else {
			printf(infoLog);
			
			free(infoLog);
		}
		
		// return null
		return NULL_SHADER;
	}
	
	// return shader
	return shader;
}

// gets the info log from a shader.  free when done using!
GLchar* getShaderInfoLog(GLuint shader){
	// get info log length
	GLint infoLogLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	if(infoLogLength == 0) return NULL;
	
	// get info log
	GLchar* infoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); // NOTE: I'm aware that chars should be 1 byte and the sizeof is technically unnecessary, but it seems like a good safety measure

	glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
	
	return infoLog;
}

// creates (and links) a shader program given a vertex shader and a fragment shader
GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader, bool deleteShaders){
	// create program
	GLuint shaderProgram = glCreateProgram();
	
	// attach shaders
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	
	// link program
	glLinkProgram(shaderProgram);
	
	if(deleteShaders){
		// delete shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	
	// check for link errors
	GLint linkStatus = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	
	if(!linkStatus){
		// get info log
		GLchar* infoLog = getShaderProgramInfoLog(shaderProgram);
		
		if(!infoLog){
			printf("There was a shader program linker error, but the info log is empty.\n");
		} else {
			printf(infoLog);
			
			free(infoLog);
		}
		
		return NULL_SHADER_PROGRAM;
	}
	
	return shaderProgram;
}

GLchar* getShaderProgramInfoLog(GLuint shaderProgram){
	// get info log length
	GLint infoLogLength = 0;
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	if(infoLogLength == 0) return NULL;
	
	// get info log
	GLchar* infoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); // NOTE: I'm aware that chars should be 1 byte and the sizeof is technically unnecessary, but it seems like a good safety measure

	glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, infoLog);
	
	return infoLog;
}

ShaderProgramEx* createShaderProgramEx(GLuint vertexShader, GLuint fragmentShader, bool deleteShaders){
	// allocate memory
	ShaderProgramEx* programEx = allocateMemoryForType<ShaderProgramEx>();
	
	// create gl shader program
	programEx->program = createShaderProgram(vertexShader, fragmentShader, deleteShaders);
	
	// make sure program compiled
	if(programEx->program == NULL_SHADER_PROGRAM){
		free(programEx);
		
		return NULL;
	}
	
	// create uniform manager (remember to delete)
	programEx->uniforms = new std::map<std::string, GLint>();
		
	// load uniforms
	loadShaderProgramExUniformLocations(programEx);
	
	// get max supported texture units
	// TODO: only needs to be called once at start of program
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &programEx->maxTextureUnits);
	
	programEx->textureUnits = 0;
	
	programEx->numPointLights = 0;
	programEx->maxPointLights = 48;
	
	return programEx;
}

// load uniform locations into the uniform manager
void loadShaderProgramExUniformLocations(ShaderProgramEx* programEx){
	// store program
	GLuint program = programEx->program;
	
	// check for valid values
	if(!program) return;
	if(!programEx->uniforms) return;
	
	// get number of active uniforms
	GLint numUniforms = 0;
	
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
	
	if(!numUniforms) return;
	
	// loop through each uniform and get its name and location
	for(GLint i = 0; i < numUniforms; i++){
		// name buffer
		// FIXME: 256 could be too small of a buffer, or too large
		char nameBuffer[256] = {'\0'};
		
		glGetActiveUniformName(program, (GLuint)i, 256, NULL, nameBuffer);
		
		// convert name to std::string
		std::string name = std::string(nameBuffer);
		
		// get location
		GLint location = glGetUniformLocation(program, name.c_str());
		
		if(location == -1) continue;
		
		// save to uniform manager
		(*programEx->uniforms)[name] = location;
	}
}

void useProgramEx(ShaderProgramEx* programEx){
	glUseProgram(programEx->program);
}

// get uniform location
GLint getProgramExUniformLocation(ShaderProgramEx* programEx, std::string name){
	return (*programEx->uniforms)[name];
}

// bind a texture to a uniform according to the number of textures currently bound
// note that this will stop working quickly if the amount of bound textures isn't reset after drawing
void setProgramExUniformTexture(ShaderProgramEx* programEx, const char* location, TextureData* textureData){
	// check if we've exceeded max bound textures
	if( (int32_t)programEx->textureUnits >= programEx->maxTextureUnits ){
		printf("Can't bind more textures, reached maximum supported texture units (%d)\n", programEx->maxTextureUnits);
		return;
	}
	
	// bind texture to active texture
	glActiveTexture(GL_TEXTURE0 + programEx->textureUnits);
	
	if(textureData) glBindTexture(GL_TEXTURE_2D, textureData->texture);
	
	// assign active texture to uniform
	glUniform1i(getProgramExUniformLocation(programEx, std::string(location)), programEx->textureUnits);

	// increment current textures
	programEx->textureUnits++;
}

// reset the number of currently bound textureUnits
// should generally be called every time something is drawn, since generally you'll be binding new textures
void resetProgramExUniformTextures(ShaderProgramEx* programEx){
	programEx->textureUnits = 0;
}

// add a new light to a uniform array of lights
// location = array of lights
void addProgramExPointLight(ShaderProgramEx* programEx, const char* location, PointLight* light){
	// check if we've exceeded max lights
	if( programEx->numPointLights >= programEx->maxPointLights ){
		printf("Can't bind more point lights, reached maximum supported point lights (%d)\n", programEx->maxPointLights);
		return;
	}
	
	// get location name + index
	std::string locationName(location);
	locationName += '[';
	locationName += std::to_string(programEx->numPointLights);
	locationName += ']';
	
	// write each value to shader
	std::string locationBuffer;
	
	// position
	locationBuffer = locationName + ".position";
	glUniform3fv(getProgramExUniformLocation(programEx, locationBuffer), 1, glm::value_ptr(light->position));
	
	// color
	locationBuffer = (locationName + ".color");
	glUniform3fv(getProgramExUniformLocation(programEx, locationBuffer), 1, glm::value_ptr(light->color));
	
	// strengths
	locationBuffer = locationName + ".ambientStrength";
	glUniform1f(getProgramExUniformLocation(programEx, locationBuffer), light->ambientStrength);
	
	locationBuffer = locationName + ".diffuseStrength";
	glUniform1f(getProgramExUniformLocation(programEx, locationBuffer), light->diffuseStrength);
	
	// attenuation values
	locationBuffer = locationName + ".c";
	glUniform1f(getProgramExUniformLocation(programEx, locationBuffer), light->c);
	
	locationBuffer = locationName + ".l";
	glUniform1f(getProgramExUniformLocation(programEx, locationBuffer), light->l);
	
	locationBuffer = locationName + ".q";
	glUniform1f(getProgramExUniformLocation(programEx, locationBuffer), light->q);
	
	// update numPointLights
	programEx->numPointLights++;
	
	glUniform1i(getProgramExUniformLocation(programEx, "numPointLights"), programEx->numPointLights);
}

void resetProgramExPointLights(ShaderProgramEx* programEx){
	programEx->numPointLights = 0;
}