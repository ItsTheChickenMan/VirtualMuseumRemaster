// contains implementations for shapes

#ifndef VMR_SHAPES_H
#define VMR_SHAPES_H

// Position {x, y, z}
float g_triangle_2D[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  

// Position {x, y, z}, UV {s, t}
float g_triangle_2D_Tex[] = {
	-0.5f, -0.5f,  0.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.0f,   1.0f, 0.0f,
	 0.0f,  0.5f,  0.0f,   0.5f, 1.0f
};

// Position {x, y, z}, UV{ s, t}, Normal {x, y, z}
float g_cube[] = {
	// back face (CCW winding)
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // bottom-right
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // top-left
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
	// front face (CCW winding)
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // bottom-left
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // bottom-left
	// left face (CCW)
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	// right face (CCW)
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	// bottom face (CCW)      
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // top-right
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // top-right
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	// top face (CCW)
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
};

struct VertexDataInfo {
	float* vertices;
	
	uint32_t vertexCount;
	uint32_t sizeInBytes;
	
	uint32_t *componentOrder;
	uint32_t numComponents;
};

uint32_t g_vertexOnlyOrder[] = {0};
uint32_t g_vertexAndTextureOrder[] = {0, 1};
uint32_t g_vertexAndTextureAndNormalsOrder[] = {0, 1, 2};
uint32_t g_vertexAndNormalsAndTextureOrder[] = {0, 2, 1};

std::map<std::string, VertexDataInfo> g_shapes {
	{"triangle_2D", (VertexDataInfo){g_triangle_2D, 3, sizeof(g_triangle_2D), g_vertexOnlyOrder, 1} },
	{"triangle_2D_Tex", (VertexDataInfo){g_triangle_2D_Tex, 3, sizeof(g_triangle_2D_Tex), g_vertexAndTextureOrder, 2} },
	{"cube", (VertexDataInfo){g_cube, 36, sizeof(g_cube), g_vertexAndNormalsAndTextureOrder, 3}}
};
	
#endif