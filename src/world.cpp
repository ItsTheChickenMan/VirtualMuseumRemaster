// world parser
#include <world.h>
#include <utils.h>
#include <shapes.h>

#include <cctype>

// textured object constructors

// creates a textured renderable object from an existing object and an existing texture data
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, TextureData* texture){
	TexturedRenderableObject* texturedObject = allocateMemoryForType<TexturedRenderableObject>();
	
	texturedObject->renderableObject = object;
	texturedObject->textureData = texture;
	
	return texturedObject;
}

// from new renderable object params and existing texture data
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, TextureData* texture){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	
	return createTexturedRenderableObject(object, texture);
}

// from existing rendearble object and new texture data params
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, const char* texturePath){
	TextureData* texture = createTextureData(texturePath);
	
	return createTexturedRenderableObject(object, texture);
}

// completely new
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const char* texturePath){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	TextureData* texture = createTextureData(texturePath);
	
	return createTexturedRenderableObject(object, texture);
}

// world

// parser settings
const char commentDelimiter = '#';
const char parameterDelimiter = ',';
const char blockOpen = '[';
const char blockClose = ']';
const char textureBlockDelimiter = '%';
const char vertexDataBlockDelimiter = '*';
const char objectBlockDelimiter = '$';
const char lightBlockDelimiter = '&';

// texture char parser	
bool textureBlockCharParser(void** block, char byte){	
	// cast pointer to block type
	TextureBlock* textureBlock = (TextureBlock*)(*block);
	
	// ensure that block exists, create it if it doesn't
	if(*block == NULL){
		textureBlock = allocateMemoryForType<TextureBlock>();
		
		// construct members
		textureBlock->strings = new std::vector<std::string>();
		textureBlock->parameterIndex = 0;
		textureBlock->parameterBuffer= new std::string();
		
		// point block to created textureBlock
		*block = (void*)textureBlock;
		
		// because this should be created on the first byte, we can safely exit here to avoid the open bracket being included
		return false;
	}
	
	// parse byte
	
	// add byte to parameter buffer if it's not equal to the parameter delimiter or the close block
	if(byte != parameterDelimiter && byte != blockClose){
		textureBlock->parameterBuffer->push_back(byte);
	} else {
		// flush parameter buffer
		textureBlock->strings->push_back( *textureBlock->parameterBuffer ); // push_back constructs a copy, so there shouldn't be anything to worry about here
		
		// reset parameterBuffer
		textureBlock->parameterBuffer->erase(0, std::string::npos);
		
		// if parameterIndex is at the maximum value, return true (done)
		if(textureBlock->parameterIndex+1 == TextureBlock::numStrings){
			// block is done parsing
			return true;
		}
		
		// otherwise, increment parameterIndex
		textureBlock->parameterIndex++;
	}
	
	return false;
}

void textureBlockToScene(void** block, Scene* scene){
	// cast block to block type
	TextureBlock* textureBlock = (TextureBlock*)(*block);
	
	// load values
	std::string texturePath = textureBlock->strings->at(0);
	std::string textureName = textureBlock->strings->at(1);
	
	// load texture
	(*scene->textures)[textureName] = createTextureData(texturePath.c_str());
	
	// delete contents of block
	// remove the parameter buffer
	delete textureBlock->parameterBuffer;
	
	// delete the parameters object
	delete textureBlock->strings;
	
	// free memory
	free(textureBlock);
	
	*block = NULL;
}

// no vertex data block parser bc it uses the TextureBlock struct

void vertexDataBlockToScene(void** block, Scene* scene){
	// cast block to block type
	TextureBlock* vertexDataBlock = (TextureBlock*)(*block);
	
	// load values
	std::string shapeName = vertexDataBlock->strings->at(0);
	std::string vertexDataName = vertexDataBlock->strings->at(1);
	
	// load vertex data
	VertexDataInfo info = g_shapes[shapeName];
	
	(*scene->vertexData)[vertexDataName] = createVertexData(info.vertices, info.vertexCount, info.sizeInBytes, info.componentOrder, info.numComponents);
	
	// delete contents of block
	// remove the parameter buffer
	delete vertexDataBlock->parameterBuffer;
	
	// delete the parameters object
	delete vertexDataBlock->strings;
	
	// free memory
	free(vertexDataBlock);
	
	*block = NULL;
}

bool objectBlockCharParser(void** block, char byte){
	// cast pointer to block type
	ObjectBlock* objectBlock = (ObjectBlock*)(*block);
	
	// ensure that block exists, create it if it doesn't
	if(*block == NULL){
		objectBlock = allocateMemoryForType<ObjectBlock>();
		
		// construct members
		objectBlock->floats = new std::vector<float>();
		objectBlock->strings = new std::vector<std::string>();
		objectBlock->parameterIndex = 0;
		objectBlock->parameterBuffer= new std::string();
		
		// point block to created textureBlock
		*block = (void*)objectBlock;
		
		// because this should be created on the first byte, we can safely exit here to avoid the open bracket being included
		return false;
	}
	
	// parse char (if it's not the delimiter or block close)
	if(byte != parameterDelimiter && byte != blockClose){
		// push to parameter buffer
		objectBlock->parameterBuffer->push_back(byte);
	} else {
		// flush parameter buffer
		
		// check what we're parsing
		if(objectBlock->parameterIndex < ObjectBlock::numFloats){
			// if parsing floats, convert the buffer to a float and push to floats
			float floatParameter = std::stof(*objectBlock->parameterBuffer);
			
			// push to floats buffer
			objectBlock->floats->push_back(floatParameter);
			
			// reset parameterBuffer
			objectBlock->parameterBuffer->erase(0, std::string::npos);
		
			// increment parameter index
			objectBlock->parameterIndex++;
		} else if(objectBlock->parameterIndex < ObjectBlock::numFloats+ObjectBlock::numStrings){
			// if parsing strings, just push the string to strings
			objectBlock->strings->push_back(*objectBlock->parameterBuffer);
			
			// reset parameterBuffer
			objectBlock->parameterBuffer->erase(0, std::string::npos);
			
			// check if this is the end
			if(objectBlock->parameterIndex+1 == ObjectBlock::numFloats+ObjectBlock::numStrings){
				// last argument reached, return true to indicate that we're done parsing
				return true;
			}
			
			// increment parameter index
			objectBlock->parameterIndex++;
		}
	}
	
	return false;
}

void objectBlockToScene(void** block, Scene* scene){
	// cast pointer to block type
	ObjectBlock* objectBlock = (ObjectBlock*)(*block);
	
	// load values
	std::string textureName = objectBlock->strings->at(0);
	std::string vertexDataName = objectBlock->strings->at(1);
	
	// check texture
	TextureData* texture = (*scene->textures)[textureName];
	
	if(!texture){
		printf("Invalid texture name %s", textureName.c_str());
		
		TextureData* def = scene->textures->at("default");
		
		if(def){
			texture = def;
			printf(", reverting to default\n");
		} else {
			printf("Invalid texture name %s with no default present\n");
			
			return; // fail
		}
	}
	
	// check vertex data
	VertexData* vData = (*scene->vertexData)[vertexDataName];
	
	if(!vData){
		printf("Invalid vertex data name %s\n", vertexDataName.c_str());
		
		return; // fail
	}
	
	float x = objectBlock->floats->at(0);
	float y = objectBlock->floats->at(1);
	float z = objectBlock->floats->at(2);
	
	float yaw = objectBlock->floats->at(3);
	float pitch = objectBlock->floats->at(4);
	float roll = objectBlock->floats->at(5);
	
	float w = objectBlock->floats->at(6);
	float h = objectBlock->floats->at(7);
	float d = objectBlock->floats->at(8);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 rotation = glm::vec3(yaw, pitch, roll);
	glm::vec3 scale = glm::vec3(w, h, d);
	
	// create object
	TexturedRenderableObject* object = createTexturedRenderableObject(vData, position, rotation, scale, texture);
	
	// push to scene
	std::vector<TexturedRenderableObject*> *objectVector = (*scene->staticObjects)[vData];
	
	if(!objectVector){
		objectVector = new std::vector<TexturedRenderableObject*>();
		
		(*scene->staticObjects)[vData] = objectVector;
	}
	
	objectVector->push_back(object);
	
	// delete contents of block
	// remove the parameter buffer
	delete objectBlock->parameterBuffer;
	
	// delete the parameters objects
	delete objectBlock->floats;
	delete objectBlock->strings;
	
	// free memory
	free(objectBlock);
	
	*block = NULL;
}

bool lightBlockCharParser(void** block, char byte){
	// cast pointer to block type
	LightBlock* lightBlock = (LightBlock*)(*block);
	
	// ensure that block exists, create it if it doesn't
	if(*block == NULL){
		lightBlock = allocateMemoryForType<LightBlock>();
		
		// construct members
		lightBlock->floats = new std::vector<float>();
		lightBlock->parameterIndex = 0;
		lightBlock->parameterBuffer= new std::string();
		
		// point block to created lightBlock
		*block = (void*)lightBlock;
		
		// because this should be created on the first byte, we can safely exit here to avoid the open bracket being included
		return false;
	}
	
	// parse byte
	
	// add byte to parameter buffer if it's not equal to the parameter delimiter or the close block
	if(byte != parameterDelimiter && byte != blockClose){
		lightBlock->parameterBuffer->push_back(byte);
	} else {
		// flush parameter buffer
		
		// if parsing floats, convert the buffer to a float and push to floats
		float floatParameter = std::stof(*lightBlock->parameterBuffer);
		
		// push to floats buffer
		lightBlock->floats->push_back(floatParameter);
		
		// reset parameterBuffer
		lightBlock->parameterBuffer->erase(0, std::string::npos);
		
		// if parameterIndex is at the maximum value, return true (done)
		if(lightBlock->parameterIndex+1 == LightBlock::numFloats){
			// block is done parsing
			return true;
		}
		
		// otherwise, increment parameterIndex
		lightBlock->parameterIndex++;
	}
	
	return false;
}

void lightBlockToScene(void** block, Scene* scene){
	// cast block to block type
	LightBlock* lightBlock = (LightBlock*)(*block);
	
	// load values
	float x = lightBlock->floats->at(0);
	float y = lightBlock->floats->at(1);
	float z	= lightBlock->floats->at(2);
	
	float r = lightBlock->floats->at(3);
	float g = lightBlock->floats->at(4);
	float b = lightBlock->floats->at(5);
	
	float c = lightBlock->floats->at(6);
	float l = lightBlock->floats->at(7);
	float q = lightBlock->floats->at(8);
	
	float as = lightBlock->floats->at(9);
	float ds = lightBlock->floats->at(10);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 color = glm::vec3(r, g, b);
	
	// create light
	PointLight* light = createPointLight(position, color, as, ds, c, l, q);
	
	// add to scene
	scene->pointLights->push_back(light);
	
	// delete contents of block
	// remove the parameter buffer
	delete lightBlock->parameterBuffer;
	
	// delete the parameters object
	delete lightBlock->floats;
	
	// free memory
	free(lightBlock);
	
	*block = NULL;
}

// create the shell of a scene
Scene* createScene(){
	// allocate memory
	Scene* scene = allocateMemoryForType<Scene>();
	
	// initialize values
	scene->vertexData = new std::map<std::string, VertexData*>();
	scene->textures = new std::map<std::string, TextureData*>();
	scene->staticObjects = new std::map<VertexData*, std::vector<TexturedRenderableObject*>*>();
	scene->pointLights = new std::vector<PointLight*>();
	
	return scene;
}

// parse world
Scene* parseWorld(const char* file){
	// file buffer
	char* fileBuffer = read_entire_file(file);
	
	if(fileBuffer == NULL){
		printf("Invalid path for world %s\n", file);
		return NULL;
	}
	
	// created scene
	Scene* scene = createScene();
	
	// settings
	char blockDelimiters[] = {textureBlockDelimiter, vertexDataBlockDelimiter, objectBlockDelimiter, lightBlockDelimiter};
	
	// control states
	bool parsingBlock = false;
	bool ignoringUntilNextLine = false;
	int32_t blockParsing = -1;
	void* blockBuffer = NULL;
	
	// pointers to char parsers
	// first param = pointer to block (void* here to be valid for all pointers)
	// second param = char to parse
	// returns false if the block hasn't finished parsing and true if it has
	bool (*charParsers[4])(void**,char) {textureBlockCharParser, textureBlockCharParser, objectBlockCharParser, lightBlockCharParser};
	void (*blockParsers[4])(void**,Scene*) {textureBlockToScene, vertexDataBlockToScene, objectBlockToScene, lightBlockToScene};
	
	/*
	charParsers[0] = textureBlockCharParser;
	charParsers[1] = objectBlockCharParser;
	charParsers[2] = lightBlockCharParser;
	*/
	
	// loop through each byte
	char byte;
	char lastByte;
	uint32_t byteIndex = 0;
	
	//printf("\n");
	
	do {
		// load byte
		byte = fileBuffer[byteIndex];
		if(byteIndex > 0) lastByte = fileBuffer[byteIndex-1];
		
		uint32_t index = byteIndex;
		byteIndex++;
		
		// if currently in a comment, check for newline
		if(ignoringUntilNextLine){
			// check for newline
			if(byte == '\n'){
				ignoringUntilNextLine = false;
			}
			
			continue;
		}
		
		// check for whitespace (ignored)
		if(isspace(byte)) continue;
		
		// check for hashtag (comment, ignores)
		if( (lastByte == '\n' || index == 0) && byte == commentDelimiter){
			ignoringUntilNextLine = true;
			continue;
		}
		
		// byte parser
		
		// if a block isn't being parsed currently, look for a delimiter
		if(blockParsing < 0){
			// look for delimiter
			for(int32_t i = 0; i < sizeof(blockDelimiters)/sizeof(char); i++){
				if(byte == blockDelimiters[i]){
					blockParsing = i;
					break;
				}
			}
			
			continue;
		}
		
		// if a block is being parsed, pass the byte to the block parser function
		bool done = (*charParsers[blockParsing])(&blockBuffer, byte);
		
		if(done){
			// parse block into scene
			(*blockParsers[blockParsing])(&blockBuffer, scene);
			
			// reset block parsing (blockBuffer is freed by block parser when done)
			blockParsing = -1;
			blockBuffer = NULL;
		}
	} while( byte != 0 ); // end at null terminator
	
	free(fileBuffer);
	
	return scene;
}