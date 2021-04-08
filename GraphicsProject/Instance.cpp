#include "Instance.h"
#include "Scene.h"
#include "OBJMesh.h"
#include "Camera.h"
#include "Shader.h"
#include <glm/ext.hpp>


Instance::Instance(glm::mat4 a_transform, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader)
{
	m_transform = a_transform;
	m_mesh = a_mesh;
	m_shader = a_shader;
}

Instance::Instance(const glm::vec3& a_position, const glm::vec3& a_eulerAngles, const glm::vec3& a_scale, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader)
{
	m_transform = createTransform(a_position, a_eulerAngles, a_scale);
	m_mesh = a_mesh;
	m_shader = a_shader;
}


void Instance::draw(Scene* a_scene)
{
	m_shader->bind();

	//bind the Projection View Matrix and model matrix
	glm::mat4 pvm = a_scene->getCurrentCamera()->getProjectionMatrix(a_scene->getWindowSize()) * a_scene->getCurrentCamera()->getViewMatrix() * m_transform;
	m_shader->bindUniform("ProjectionViewModel", pvm);
	m_shader->bindUniform("ModelMatrix", m_transform);
	//lighting and camera pos are set by Scene, as they are the same for all objects
	
	m_mesh->draw();
}

glm::mat4 Instance::createTransform(const glm::vec3& a_position, const glm::vec3& a_eulerAngles, const glm::vec3& a_scale)
{
	//use glm functions to create transform matrix
	return glm::translate(glm::mat4(1), a_position) 
		* glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.z), glm::vec3(0, 0, 1)) 
		* glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.y), glm::vec3(0, 1, 0)) 
		* glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.x), glm::vec3(1, 0, 0)) 
		* glm::scale(glm::mat4(1), a_scale);
}
