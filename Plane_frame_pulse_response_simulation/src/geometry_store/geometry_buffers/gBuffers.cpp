#include "gBuffers.h"

gBuffers::gBuffers()
{
	// Empty constructor
}

gBuffers::~gBuffers()
{
	// Empty destructor
}

void gBuffers::Bind() const
{
	// Bind the buffers
	vao.Bind();
	ibo.Bind();
}

void gBuffers::UnBind() const
{
	// Un Bind the buffers
	vao.UnBind();
	ibo.UnBind();
}

void gBuffers::CreateBuffers(const float* vb_data, unsigned int& vb_size, 
	const unsigned int* ib_indices, unsigned int& ib_count, VertexBufferLayout& vb_layout)
{
	vao.createVertexArray();

	// Vertex buffer (vertices and number of vertices * sizeof(float)
	vbo.createVertexBuffer(vb_data, vb_size);

	// Index buffer (indices and number of indices)
	ibo.createIndexBuffer(ib_indices, ib_count);

	// Vertex Array (vertex buffer and vertex buffer layout) 
	vao.AddBuffer(vbo, vb_layout);
}
