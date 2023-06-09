#pragma once
#include "indexBuffer.h"
#include "VertexArray.h"

// Below header files are not used in this class 
#include "Shader.h"
#include "Texture.h"


class gBuffers
{
public:
	gBuffers();
	~gBuffers();
	void Bind()const;
	void UnBind()const;
	void CreateBuffers(const float* vb_data,
		unsigned int& vb_size,
		const unsigned int* ib_indices,
		unsigned int& ib_count,
		VertexBufferLayout& vb_layout);
	void CreateDynamicBuffers(const float* vb_data,
		unsigned int& vb_size,
		const unsigned int* ib_indices,
		unsigned int& ib_count,
		VertexBufferLayout& vb_layout);
	VertexBuffer vbo;
	VertexArray vao;
	IndexBuffer ibo;
private:

};