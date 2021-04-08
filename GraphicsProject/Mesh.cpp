#include <gl_core_4_4.h>
#include "Mesh.h"

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

void Mesh::initialiseQuad()
{
	//check mesh not already initialised
	assert(m_vao == 0);

	//generate buffers
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	// bind vertex array; a mesh wrapper
	glBindVertexArray(m_vao);

	//bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	//define the 6 verticies for the two triangles of a quad
	Vertex vertices[6];
	vertices[0].m_position = { -0.5f, 0.f, 0.5f,  1.f };
	vertices[1].m_position = { 0.5f,  0.f, 0.5f,  1.f };
	vertices[2].m_position = { -0.5f, 0.f, -0.5f, 1.f };

	vertices[3].m_position = { -0.5f, 0.f, -0.5f, 1.f };
	vertices[4].m_position = { 0.5f,  0.f, 0.5f,  1.f };
	vertices[5].m_position = { 0.5f,  0.f, -0.5f, 1.f };

	//all normals face up
	vertices[0].m_normal = { 0, 1, 0, 0 };
	vertices[1].m_normal = { 0, 1, 0, 0 };
	vertices[2].m_normal = { 0, 1, 0, 0 };
	vertices[3].m_normal = { 0, 1, 0, 0 };
	vertices[4].m_normal = { 0, 1, 0, 0 };
	vertices[5].m_normal = { 0, 1, 0, 0 };

	vertices[0].m_texCoord = { 0, 1 };	//bot left
	vertices[1].m_texCoord = { 1, 1 };	//bot right
	vertices[2].m_texCoord = { 0, 0 };	//top left
	vertices[3].m_texCoord = { 0, 0 };	//top left
	vertices[4].m_texCoord = { 1, 1 };	//bot right
	vertices[5].m_texCoord = { 1, 0 };	//top right


	//fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	//glVertexAttribPointer( location, size, type, normalized?, gap, (first index * size of index(GLuint)) );

	//enable the first element as position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//enable the second element as normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)16);
	//enable the third element as texCoord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)32);

	//unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//quad has two triangles
	m_triCount = 2;
}

void Mesh::Initialise(unsigned int a_vertexCount, const Vertex* a_vertices, unsigned int a_indexCount, unsigned int* a_indices)
{
	//check mesh not already initialised
	assert(m_vao == 0);

	//generate buffers
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	// bind vertex array; a mesh wrapper
	glBindVertexArray(m_vao);

	//bind and fill vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, a_vertexCount * sizeof(Vertex), a_vertices, GL_STATIC_DRAW);

	//enable first element as position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	//bind indices if they exist
	if (a_indexCount != 0)
	{
		glGenBuffers(1, &m_ibo);

		//bind and fill vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);

		m_triCount = a_indexCount / 3;
	}
	else
	{
		m_triCount = a_vertexCount / 3;
	}

	//unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw()
{
	glBindVertexArray(m_vao);

	//check if we are using indices or verticies
	if (m_ibo != 0)
	{
		glDrawElements(GL_TRIANGLES, 3 * m_triCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 3 * m_triCount);
	}
}
