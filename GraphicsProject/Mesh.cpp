#include <gl_core_4_4.h>
#include "Mesh.h"

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Mesh::initialiseQuad()
{
	//check mesh not already initialised
	assert(vao == 0);

	//generate buffers
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// bind vertex array; a mesh wrapper
	glBindVertexArray(vao);

	//bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//define the 6 verticies for the two triangles of a quad
	Vertex vertices[6];
	vertices[0].position = { -0.5f, 0.f, 0.5f,  1.f };
	vertices[1].position = { 0.5f,  0.f, 0.5f,  1.f };
	vertices[2].position = { -0.5f, 0.f, -0.5f, 1.f };

	vertices[3].position = { -0.5f, 0.f, -0.5f, 1.f };
	vertices[4].position = { 0.5f,  0.f, 0.5f,  1.f };
	vertices[5].position = { 0.5f,  0.f, -0.5f, 1.f };

	//fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	//enable first element as a position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	//unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//quad has two triangles
	triCount = 2;
}

void Mesh::draw()
{
	glBindVertexArray(vao);

	//check if we are using indices or verticies
	if (ibo != 0)
	{
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	}
}
