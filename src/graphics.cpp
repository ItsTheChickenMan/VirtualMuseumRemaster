// graphics manager/window manager

// includes //

#include <graphics.h>
#include <utils.h>
#include <camera.h>
#include <shader.h>

#include <string>

// window management //

// initialize glfw/glad
// should only need to be called once at the start of the program
InitGraphicsStatus initGraphics(){
	// initialize glfw
	if(glfwInit() != GL_TRUE){
		return GLFW_INIT_ERROR;
	}
	
	// set glfw window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opengl version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // core profile
	
	// temporarily make non-resizeable to avoid the render issues that causes
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	return SUCCESS;
}

// kill glfw/other graphics stuff
void terminateGraphics(){
	glfwTerminate();
}

// create a new render window
// returns NULL if an error occurred
Window* createWindow(int32_t width, int32_t height, const char* title){
	// allocate memory for window
	Window* window = allocateMemoryForType<Window>();
	
	// create glfw window
	window->glfwWindow = (GLFWwindow*)glfwCreateWindow(width, height, title, NULL, NULL);
	
	// check to ensure window created properly
	if(!window->glfwWindow){
		terminateGraphics();
		return NULL;
	}
	
	// make window current context
	glfwMakeContextCurrent(window->glfwWindow);
	
	// load gl methods
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		terminateGraphics();
		return NULL;
	}
	
	// set viewport size
	glViewport(0, 0, width, height);
	
	// default settings
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glfwSwapInterval(1);
	
	return window;
}

// if the window has received some kind of close event
bool shouldWindowClose(Window* window){
	return glfwWindowShouldClose(window->glfwWindow);
}

void updateWindow(Window* window){
	// swap buffers
	glfwSwapBuffers(window->glfwWindow);
	
	// poll for events
	glfwPollEvents();
}

// clear the window and replace with a color
// also flushes the depth map
void clearWindow(float r, float g, float b){
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// vertex //
Vertex createVertex(glm::vec3 position, glm::vec2 textureCoordinates, glm::vec3 normal){
	return (Vertex){position, textureCoordinates, normal};
}

// vertex data //

// create vertex data from a set of vertices
// if vertexCount is not equal to sizeof(vertices)/sizeof(float), there will be problems
// for component order: 
// 0 = 3-component vertices
// 1 = 2-component texture coordinates
// 2 = 3-component normals
VertexData* createVertexData(float *vertices, uint32_t vertexCount, uint32_t sizeInBytes, uint32_t *componentOrder, uint32_t numComponents){
	// allocate space
	VertexData* data = allocateMemoryForType<VertexData>();
	
	// set default values in case any methods below fail
	data->vbo = 0;
	data->vao = 0;
	data->ebo = 0;
	data->indexCount = 0;
	data->vertexCount = vertexCount;
	data->sizeInBytes = sizeInBytes;
	
	// create vertex buffer object
	glGenBuffers(1, &data->vbo);
	
	// check for success
	// FIXME: actual error checking using gl methods
	if(data->vbo == 0) return NULL;
	
	// bind buffer to array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	
	// copy vertex data to buffer
	glBufferData(GL_ARRAY_BUFFER, data->sizeInBytes, vertices, GL_STATIC_DRAW); // target, size, data, purpose
	
	// generate vertex attribute object
	glGenVertexArrays(1, &data->vao);
	
	// check for success
	if(data->vao == 0) return NULL;
	
	// bind vao
	glBindVertexArray(data->vao);
	
	// total stride for each element
	uint32_t stride = 0;
	
	// calculate total size of stride
	for(uint32_t i = 0; i < numComponents; i++){
		uint32_t component = componentOrder[i];
		uint32_t componentSize = 3 - (component % 2); // this will need to be changed should any more components be supported
		
		stride += componentSize;
	}
	
	// offset of the current component
	uint32_t offset = 0;
	
	// create vertex attribues
	for(uint32_t i = 0; i < numComponents; i++){
		uint32_t component = componentOrder[i];
		uint32_t componentSize = 3 - (component % 2); // this will need to be changed should any more components be supported
		
		glVertexAttribPointer(component, componentSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset*sizeof(float)));
		glEnableVertexAttribArray(component);
		
		offset += componentSize;
	}
	
	// unbind buffer and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return data;
}

VertexData* createVertexData(Vertex *vertices, uint32_t vertexCount, uint32_t sizeInBytes){
	// allocate space
	VertexData* data = allocateMemoryForType<VertexData>();
	
	// set default values in case any methods below fail
	data->vbo = 0;
	data->vao = 0;
	data->ebo = 0;
	data->vertexCount = vertexCount;
	data->indexCount = 0;
	data->sizeInBytes = sizeInBytes;
	
	// create vertex buffer object
	glGenBuffers(1, &data->vbo);
	
	// check for success
	// FIXME: actual error checking using gl methods
	if(data->vbo == 0) return NULL;
	
	// bind buffer to array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	
	// copy vertex data to buffer
	glBufferData(GL_ARRAY_BUFFER, data->sizeInBytes, vertices, GL_STATIC_DRAW); // target, size, data, purpose
	
	// generate vertex attribute object
	glGenVertexArrays(1, &data->vao);
	
	// check for success
	if(data->vao == 0) return NULL;
	
	// bind vao
	glBindVertexArray(data->vao);
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	
	// tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// unbind buffer and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return data;
}

VertexData* createVertexData(std::vector<Vertex> vertices){
	return createVertexData(vertices, std::vector<uint32_t>());
}

VertexData* createVertexData(std::vector<Vertex> vertices, std::vector<uint32_t> indices){
	// allocate space
	VertexData* data = allocateMemoryForType<VertexData>();
	
	// set default values in case any methods below fail
	data->vbo = 0;
	data->vao = 0;
	data->ebo = 0;
	data->vertexCount = vertices.size();
	data->indexCount = indices.size();
	data->sizeInBytes = data->vertexCount * sizeof(Vertex);
	
	// create vertex buffer object
	glGenBuffers(1, &data->vbo);
	
	// check for success
	// FIXME: actual error checking using gl methods
	if(data->vbo == 0) return NULL;
	
	// bind buffer to array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	
	// copy vertex data to buffer
	glBufferData(GL_ARRAY_BUFFER, data->sizeInBytes, &vertices[0], GL_STATIC_DRAW); // target, size, data, purpose
	
	// generate vertex attribute object
	glGenVertexArrays(1, &data->vao);
		
	// check for success
	if(data->vao == 0) return NULL;
	
	// bind vao
	glBindVertexArray(data->vao);
	
	// generate indices if they exist
	if(indices.size() > 0){
		// generate elements buffer
		glGenBuffers(1, &data->ebo);
		
		// check for success
		if(data->ebo == 0) return NULL;
		
		// copy indices data to buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	}
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	
	// tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// unbind buffer(s) and array
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	return data;
}


// bind vertex data vao
void bindVertexData(VertexData* data){
	glBindVertexArray(data->vao);
}

// bind vertex data and call draw arrays
void renderVertexData(VertexData* data){
	// bind vao
	bindVertexData(data);
	
	// call draw arrays
	renderVertexDataNoBind(data);
	
	// unbind vertex array for neatness
	glBindVertexArray(0);
}

// call draw arrays with no bind
void renderVertexDataNoBind(VertexData* data){
	// call draw arrays or elements
	if(data->ebo == 0){
		glDrawArrays(GL_TRIANGLES, 0, data->vertexCount);
	} else {
		//printf("drawing elements... %d\n", data->indexCount);
		glDrawElements(GL_TRIANGLES, data->indexCount, GL_UNSIGNED_INT, 0);
		//printf("done.\n");
	}
}

// create a renderable object (vertex data w/ model matrix)
RenderableObject* createRenderableObject(VertexData* vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){
	// allocate memory
	RenderableObject* object = allocateMemoryForType<RenderableObject>();
	
	// copy values
	object->vertexData = vertexData;
	
	// assign transform
	setRenderableObjectTransform(object, position, rotation, scale);
	
	return object;
}

// assign translation, rotation, and scale values to an object's transform
void setRenderableObjectTransform(RenderableObject* object, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){
	// store values
	object->position = position;
	object->rotation = rotation;
	object->scale = scale;
	
	// translate
	object->modelMatrix = glm::mat4(1.0f);
	
	object->modelMatrix = glm::translate(object->modelMatrix, position);
	
	// rotate around each axis
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.z, glm::vec3(0.f, 0.f, 1.f));
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.y, glm::vec3(0.f, 1.f, 0.f));
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.x, glm::vec3(1.f, 0.f, 0.f));
	
	// scale
	object->modelMatrix = glm::scale(object->modelMatrix, object->scale);
}

// render a renderable object with respect to a perspective camera's transform
// this method assumes that the shader has a pvm uniform, and has already been bound (this method will not call glUseProgram)
void renderRenderableObject(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// create pvm
	glm::mat4 pvm = camera->pv * object->modelMatrix;
	
	// assign uniforms
	glUniformMatrix4fv(getProgramExUniformLocation(programEx, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
	
	// render vertex data
	renderVertexData(object->vertexData);
}

// render a renderable object with respect to a perspective camera's transform
// this method assumes that the shader has a pvm uniform, and has already been bound (this method will not call glUseProgram)
// calls renderVertexDataNoBind
void renderRenderableObjectNoBind(RenderableObject* object, PerspectiveCamera* camera, ShaderProgramEx* programEx){
	// create pvm
	glm::mat4 pvm = camera->pv * object->modelMatrix;
	
	// assign uniforms
	glUniformMatrix4fv(getProgramExUniformLocation(programEx, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
	
	// render vertex data
	renderVertexDataNoBind(object->vertexData);
}

// create a mesh
Mesh* createMesh(VertexData* vertexData, TextureData* texture, glm::vec3 color){
	Mesh* mesh = allocateMemoryForType<Mesh>();
	
	mesh->vertexData = vertexData;
	mesh->texture = texture;
	mesh->color = color;
	
	return mesh;
}

// create a skeleton model
Model* createModel(){
	Model* model = allocateMemoryForType<Model>();
	
	model->meshes = new std::vector<Mesh*>();
	model->textures = new std::map<std::string, TextureData*>();
	model->path = NULL;
	
	return model;
}

#include <iostream>

// process mesh into a Mesh struct
void processMesh(aiMesh* aiMesh, const aiScene* scene, Model* model){
	// null checks
	if(aiMesh == NULL || model == NULL) return;
	
	// vectors for vertex data and indices, and a spot for texture/color
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	TextureData* texture = NULL;
	glm::vec3 color = glm::vec3(0);
	
	// convert aiVector3Ds to glm::vec3s and create vertices
	for(uint32_t i = 0; i < aiMesh->mNumVertices; i++){
		aiVector3D aiPosition = aiMesh->mVertices[i];
		
		glm::vec3 position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
		
		glm::vec3 normal = glm::vec3(1, 0, 0);
		
		if(aiMesh->HasNormals()){
			aiVector3D aiNormals = aiMesh->mNormals[i];
		
			normal = glm::vec3(aiNormals.x, aiNormals.y, aiNormals.z);
		}
		
		glm::vec2 textureCoords = glm::vec2(0);
		
		if(aiMesh->HasTextureCoords(0)){
			textureCoords = glm::vec2(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
		}
		
		Vertex v = createVertex(position, textureCoords, normal);
		
		vertices.push_back(v);
	}
	
	// create indices
	for(unsigned int i = 0; i < aiMesh->mNumFaces; i++){
		aiFace face = aiMesh->mFaces[i];
		
		for(unsigned int j = 0; j < face.mNumIndices; j++){
			indices.push_back(face.mIndices[j]);
		}
	}
	
	// if the mesh has vertex colors defined, use those
	if(aiMesh->HasVertexColors(0)){
		aiColor4D* colorSet = aiMesh->mColors[0];
		
		for(uint32_t i = 0; i < aiMesh->mNumVertices; i++){
			aiColor4D aiColor = colorSet[i];
			
			color = glm::vec3(aiColor.r, aiColor.g, aiColor.b);
		}
	} else if(aiMesh->mMaterialIndex >= 0){ // ptherwise load texture (if available and unloaded)
		aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
		
		// get texture
		if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0){ 
			// get path to texture
			aiString aiRelativePath;
			
			material->GetTexture(aiTextureType_DIFFUSE, 0, &aiRelativePath);
			
			std::string relativePath(aiRelativePath.C_Str());
			
			// check if texture is loaded
			if(model->textures->find(relativePath) != model->textures->end()){
				// exists
				texture = model->textures->at(relativePath);
			} else {	
				// check if texture is embedded
				const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(aiRelativePath.C_Str());
				
				if(embeddedTexture != NULL){
					// check if we need to run through stbi
					if(embeddedTexture->mHeight == 0){
						// get texture data from stbi
						texture = createTextureDataRawCompressed((unsigned char*)embeddedTexture->pcData, embeddedTexture->mWidth);
					} else {
						// FIXME: code
						printf("Error: I wish this supported raw embedded texture data, but it doesn't\n");
						texture = NULL;
					}
				} else {
					// not embedded
					texture = createTextureData( (*model->path + relativePath).c_str() );
				}
				
				(*model->textures)[relativePath] = texture;
			}
		}
	}
	
	// create vertex data
	VertexData* data = createVertexData(vertices, indices);
	
	// create mesh
	Mesh* mesh = createMesh(data, texture, color);
	
	// push mesh to model
	model->meshes->push_back(mesh);
}

// process node into meshes, etc.
void processNode(aiNode* node, const aiScene* scene, Model* model){
	// load meshes
	for(uint32_t i = 0; i < node->mNumMeshes; i++){
		uint32_t meshIndex = node->mMeshes[i];
		
		aiMesh* mesh = scene->mMeshes[meshIndex];
		
		processMesh(mesh, scene, model);
	}
	
	// process children
	for(uint32_t i = 0; i < node->mNumChildren; i++){
		aiNode* child = node->mChildren[i];
		
		processNode(child, scene, model);
	}
}

// load multiple models from vector of paths
void loadModels(std::vector<Model*>* models, std::string paths[], uint32_t numPaths){
	// instantiate importer
	Assimp::Importer importer;
	
	// load each model
	for(uint32_t i = 0; i < numPaths; i++){
		std::string& path = paths[i];
		
		Model* model = loadModel(importer, path);
		
		models->push_back(model);
	}
}

// load a model from a file
Model* loadModel(Assimp::Importer& importer, const std::string& path){
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords/* | aiProcess_FlipUVs*/); // FIXME: flip uvs?
	
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		printf("error while loading model %s: %s\n", path.c_str(), importer.GetErrorString());
		return NULL;
	}
	
	// create model
	Model* model = createModel();
	
	model->path = new std::string(path);
	
	// process the root node (also recursively processess everything else)
	processNode(scene->mRootNode, scene, model);
	
	return model;
}

// load a model from a file, creating a new importer
Model* loadModel(const std::string& path){
	// instantiate importer
	Assimp::Importer importer;
	
	// pass it to other load model method
	return loadModel(importer, path);
}