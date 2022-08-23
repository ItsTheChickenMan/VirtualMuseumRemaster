// world parser
#include <world.h>
#include <utils.h>
#include <shapes.h>

#include <cctype>

#include <stdexcept>

// textured object constructors

// creates a textured renderable object from an existing object and an existing texture data
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, TextureData* texture){
	TexturedRenderableObject* texturedObject = allocateMemoryForType<TexturedRenderableObject>();
	
	texturedObject->renderableObject = object;
	texturedObject->textureData = texture;
	texturedObject->color = glm::vec3(0);
	texturedObject->visible = true;
	
	return texturedObject;
}

// creates a textured renderable object from an existing object and vec3 color
TexturedRenderableObject* createTexturedRenderableObject(RenderableObject* object, glm::vec3 color){
	TexturedRenderableObject* texturedObject = allocateMemoryForType<TexturedRenderableObject>();
	
	texturedObject->renderableObject = object;
	texturedObject->textureData = NULL;
	texturedObject->color = color;
	texturedObject->visible = true;
	
	return texturedObject;
}

// from new renderable object params and existing texture data
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, TextureData* texture){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	
	return createTexturedRenderableObject(object, texture);
}

// from new renderable object params and color
TexturedRenderableObject* createTexturedRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color){
	RenderableObject* object = createRenderableObject(vertexData, position, rotation, scale);
	
	return createTexturedRenderableObject(object, color);
}

// from existing renderable object and new texture data params
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

// bounding box constructors

// create bounding box with 2d position
BoundingBox* createBbox(glm::vec2 p, glm::vec2 s){
	return createBbox(glm::vec3(p.x, 0, p.y), s);
}

// create bounding box with 3d position
BoundingBox* createBbox(glm::vec3 p, glm::vec2 s){
	// create 2d corners
	glm::vec2 UL = glm::vec2( (p.x-s.x/2.f), (p.z-s.y/2.f) );
	glm::vec2 UR = glm::vec2( (p.x+s.x/2.f), (p.z-s.y/2.f) );
	glm::vec2 BL = glm::vec2( (p.x-s.x/2.f), (p.z+s.y/2.f) );
	glm::vec2 BR = glm::vec2( (p.x+s.x/2.f), (p.z+s.y/2.f) );
	
	// allocate space for box
	BoundingBox* box = allocateMemoryForType<BoundingBox>();
	box->position = p;
	box->size = s;
	
	box->UL = UL;
	box->UR = UR;
	box->BL = BL;
	box->BR = BR;
	
	box->adjacent = new std::vector<uint32_t>();
	
	return box;
}

// copy bbox (but doesn't copy adjacent)
BoundingBox* createBbox(BoundingBox* original){
	return createBbox(original->position, original->size);
}

// destroy a bbox
void destroyBbox(BoundingBox* b){
	delete b->adjacent;
	
	free(b);
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
const char modelBlockDelimiter = '+';
const char walkBoxBlockDelimiter = '~';
const char settingsBlockDelimiter = '@';

// create a block
Block* createBlock(){
	Block* block = allocateMemoryForType<Block>();
	
	// construct members
	block->strings = new std::vector<std::string>();
	block->numbers = new std::vector<float>();
	block->parameterBuffer = new std::string();
	
	block->parameterIndex = 0;
	
	return block;
}

// empties out buffers and vectors, but keeps memory allocated
void emptyBlock(Block* block){
	// erase parameter buffer
	block->parameterBuffer->clear();
	
	// empty vectors
	block->strings->clear();
	block->numbers->clear();
	
	// reset parameter index
	block->parameterIndex = 0;
}

// completely deletes occupied memory
void destroyBlock(Block* block){
	// destruct members
	delete block->strings;
	delete block->numbers;
	delete block->parameterBuffer;
	
	free(block);
}

// block char parser
bool blockCharParser(Block* block, char byte){
	// ensure that block exists, create it if it doesn't
	if(block == NULL){
		// weird problem, give up
		return false;
	}
	
	// add byte to parameter buffer if it's not equal to the parameter delimiter or the close block
	if(byte != parameterDelimiter && byte != blockClose){
		block->parameterBuffer->push_back(byte);
	} else {
		// flush parameter buffer
		if(block->parameterBuffer->length() > 0 && isStringNumber( *block->parameterBuffer )){
			block->numbers->push_back( std::stof(*block->parameterBuffer) );
		} else {
			block->strings->push_back( *block->parameterBuffer );
		}
		
		// reset parameterBuffer
		block->parameterBuffer->clear();
		
		// if parameterIndex is at the maximum value, return true (done)
		if(byte == blockClose){
			// block is done parsing
			return true;
		}
		
		// otherwise, increment parameterIndex
		block->parameterIndex++;
	}
	
	return false;
}

// block to scene methods
void textureBlockToScene(Block* block, Scene* scene){
	// load values
	std::string texturePath = block->strings->at(0);
	std::string textureName = block->strings->at(1);
	
	// load texture
	(*scene->textures)[textureName] = createTextureData(texturePath.c_str());
}

void vertexDataBlockToScene(Block* block, Scene* scene){
	// load values
	std::string shapeName = block->strings->at(0);
	std::string vertexDataName = block->strings->at(1);
	
	// load vertex data
	VertexDataInfo info = g_shapes[shapeName];
	
	(*scene->vertexData)[vertexDataName] = createVertexData(info.vertices, info.vertexCount, info.sizeInBytes, info.componentOrder, info.numComponents);
}

void objectBlockToScene(Block* block, Scene* scene){
	// load some float values
	if(block->numbers->size() < 9){
		printf("Not enough enough parameters in an object block (only %d numbers and %d strings present)\n", block->numbers->size(), block->strings->size());
		return;
	}
	
	float x = block->numbers->at(0);
	float y = block->numbers->at(1);
	float z = block->numbers->at(2);
	
	float rx = block->numbers->at(3);
	float ry = block->numbers->at(4);
	float rz = block->numbers->at(5);
	
	float w = block->numbers->at(6);
	float h = block->numbers->at(7);
	float d = block->numbers->at(8);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 rotation = glm::vec3(glm::radians(rx), glm::radians(ry), glm::radians(rz));
	glm::vec3 scale = glm::vec3(w, h, d);
	
	// check amount of strings
	uint32_t stringParams = block->strings->size();
	
	// mode depends on number of string parameters (1 = model, 2 = texture + vertexData)
	switch(stringParams){
		case 0: {
			printf("There weren't enough string parameters in an object block (only %d present)\n", block->strings->size());
			return;
		}
		
		case 1: {
			// load values
			std::string modelName = block->strings->at(0);
			
			// load model
			Model* model = (*scene->models)[modelName];
			
			// check model
			if(!model){
				// FIXME: fix the various potential memory leaks in these methods when exiting too early to delete heap memory
				printf("Invalid model name %s\n", modelName.c_str());
				return; // fail
			}
			
			// split model in textured renderable objects
			for(uint32_t i = 0; i < model->meshes->size(); i++){
				Mesh* mesh = model->meshes->at(i);
				
				// create renderable object from mesh
				RenderableObject* object = createRenderableObject(mesh->vertexData, position, rotation, scale);
				
				// create textured renderable object from renderable object and texture/color
				TexturedRenderableObject* texturedObject;
				
				if(mesh->texture){
					texturedObject = createTexturedRenderableObject(object, mesh->texture);
				} else {
					texturedObject = createTexturedRenderableObject(object, mesh->color);
				}
				
				// push to scene
				std::vector<TexturedRenderableObject*>* objectVector = (*scene->staticObjects)[mesh->vertexData];
				
				if(!objectVector){
					objectVector = new std::vector<TexturedRenderableObject*>();
					
					(*scene->staticObjects)[mesh->vertexData] = objectVector;
				}
				
				objectVector->push_back(texturedObject);
			}
			
			break;
		}
		case 2: {
			// load values
			std::string textureName = block->strings->at(0);
			std::string vertexDataName = block->strings->at(1);
			
			// check texture
			TextureData* texture = (*scene->textures)[textureName];
			
			if(!texture){
				printf("Invalid texture name %s", textureName.c_str());
				
				TextureData* def = (*scene->textures)["default"];
				
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
			
			// create object
			TexturedRenderableObject* object = createTexturedRenderableObject(vData, position, rotation, scale, texture);
			
			// push to scene
			std::vector<TexturedRenderableObject*>* objectVector = (*scene->staticObjects)[vData];
			
			if(!objectVector){
				objectVector = new std::vector<TexturedRenderableObject*>();
				
				(*scene->staticObjects)[vData] = objectVector;
			}
			
			objectVector->push_back(object);
			
			break;
		}
	}
}

void lightBlockToScene(Block* block, Scene* scene){
	// load values
	
	if(block->numbers->size() < 11){
		printf("Not enough parameters for light block (only %d numbers present)\n", block->numbers->size());
		return;
	}
	
	float x = block->numbers->at(0);
	float y = block->numbers->at(1);
	float z	= block->numbers->at(2);
	
	float r = block->numbers->at(3);
	float g = block->numbers->at(4);
	float b = block->numbers->at(5);
	
	float c = block->numbers->at(6);
	float l = block->numbers->at(7);
	float q = block->numbers->at(8);
	
	float as = block->numbers->at(9);
	float ds = block->numbers->at(10);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 color = glm::vec3(r, g, b);
	
	// create light
	PointLight* light = createPointLight(position, color, as, ds, c, l, q);
	
	// add to scene
	scene->pointLights->push_back(light);
}

void modelBlockToScene(Block* block, Scene* scene){
	// load values
	std::string path = block->strings->at(0);
	std::string modelName = block->strings->at(1);
	
	// load model
	Model* model = loadModel(path);
	
	if(!model){
		return; // fail
	}
	
	(*scene->models)[modelName] = model;
}

void walkBoxBlockToScene(Block* block, Scene* scene){
	// load values
	float x = block->numbers->at(0);
	float y = block->numbers->at(1);
	float z	= block->numbers->at(2);
	
	float w = block->numbers->at(3);
	float d = block->numbers->at(4);
	
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec2 size = glm::vec2(w, d);
	
	// create bounding box
	BoundingBox* box = createBbox(position, size);
	
	// add adjacents
	for(uint32_t i = 5; i < block->numbers->size(); i++){
		// convert float to int index
		uint32_t index = (uint32_t)block->numbers->at(i);
		
		// add index to box adjacents
		box->adjacent->push_back(index);
	}
	
	// add to scene
	scene->walkmap->push_back(box);
}

void settingsBlockToScene(Block* block, Scene* scene){
	// block for misc settings
	
	// because memory is sequential in structs, this acts as an array of settings
	float* settings = &(scene->playerHeight);
	
	// loop through numbers and load them to settings
	for(uint32_t i = 0; i < block->numbers->size(); i++){
		settings[i] = block->numbers->at(i);
	}
}

// create the shell of a scene
Scene* createScene(){
	// allocate memory
	Scene* scene = allocateMemoryForType<Scene>();
	
	// initialize values
	scene->vertexData = new std::map<std::string, VertexData*>();
	scene->textures = new std::map<std::string, TextureData*>();
	scene->models = new std::map<std::string, Model*>();
	scene->staticObjects = new std::map<VertexData*, std::vector<TexturedRenderableObject*>*>();
	scene->pointLights = new std::vector<PointLight*>();
	scene->walkmap = new std::vector<BoundingBox*>();
	
	// default settings
	scene->playerHeight = 2.f;
	scene->playerRadius = 0.5f;
	scene->stepHeight = 0.4f;
	scene->maxPlayerSpeed = 2.f;
	
	return scene;
}

// parse a world file into an existing scene
void parseWorldIntoScene(Scene* scene, const char* file){
	// file buffer
	char* fileBuffer = read_entire_file(file);
	
	if(fileBuffer == NULL){
		printf("Invalid path for world %s\n", file);
		return;
	}
	
	// settings
	char blockDelimiters[] = {textureBlockDelimiter, vertexDataBlockDelimiter, objectBlockDelimiter, lightBlockDelimiter, modelBlockDelimiter, walkBoxBlockDelimiter, settingsBlockDelimiter};
	
	// control states
	bool parsingBlock = false;
	bool ignoringUntilNextLine = false;
	int32_t blockParsing = -1;
	Block* blockBuffer = createBlock();
	
	// pointers to char parsers
	// first param = pointer to block (void* here to be valid for all pointers)
	// second param = char to parse
	// returns false if the block hasn't finished parsing and true if it has
	void (*blockParsers[7])(Block*,Scene*) {textureBlockToScene, vertexDataBlockToScene, objectBlockToScene, lightBlockToScene, modelBlockToScene, walkBoxBlockToScene, settingsBlockToScene};
	
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
					
					// skip block open
					byteIndex++;
					
					break;
				}
			}
			
			continue;
		}
		
		// if a block is being parsed, pass the byte to the block parser function
		bool done = blockCharParser(blockBuffer, byte);
		
		if(done){
			// parse block into scene
			(*blockParsers[blockParsing])(blockBuffer, scene);
			
			// reset block parsing (blockBuffer is freed by block parser when done)
			blockParsing = -1;
			
			// empty the block
			emptyBlock(blockBuffer);
		}
	} while( byte != 0 ); // end at null terminator
	
	free(fileBuffer);
	
	destroyBlock(blockBuffer);
}

// parse world
Scene* parseWorld(const char* file){
	Scene* scene = createScene();
	
	parseWorldIntoScene(scene, file);
	
	return scene;
}

bool hasWalkmap(Scene* scene){
	return scene->walkmap->size() > 0;
}