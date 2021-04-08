/*  Created: 10/3/2021
 *  Author: Thomas Dufresne
 *
 *  Last Modified: 8/4/2021
 */
#pragma once
#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh() : m_triCount(0), m_vao(0), m_vbo(0), m_ibo(0) {};
	virtual ~Mesh();

	struct Vertex
	{
		glm::vec4 m_position;
		glm::vec4 m_normal;
		glm::vec2 m_texCoord;
	};

	void initialiseQuad();
	void Initialise(unsigned int a_vertexCount, const Vertex* a_vertices, unsigned int a_indexCount = 0, unsigned int* a_indices = nullptr);

	virtual void draw();

protected:
	unsigned int m_triCount;
	unsigned int m_vao,	// Vertex Array Object
				 m_vbo,	// Vertex Buffer Object
				 m_ibo;	// Index Buffer Object

};