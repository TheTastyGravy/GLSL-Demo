#include "Instance.h"
#include "Scene.h"
#include "OBJMesh.h"
#include "Camera.h"
#include "Shader.h"
#include <glm/ext.hpp>


Instance::Instance(glm::mat4 transform, aie::OBJMesh* mesh, aie::ShaderProgram* shader)
{
	this->transform = transform;
	this->mesh = mesh;
	this->shader = shader;
}

Instance::Instance(const glm::vec3& position, const glm::vec3& eulerAngles, const glm::vec3& scale, aie::OBJMesh* mesh, aie::ShaderProgram* shader)
{
	this->transform = createTransform(position, eulerAngles, scale);
	this->mesh = mesh;
	this->shader = shader;
}


void Instance::draw(Scene* scene)
{
	shader->bind();

	//bind the Projection View Matrix and model matrix
	glm::mat4 pvm = scene->getCurrentCamera()->getProjectionMatrix(scene->getWindowSize().x, scene->getWindowSize().y) * scene->getCurrentCamera()->getViewMatrix() * transform;
	shader->bindUniform("ProjectionViewModel", pvm);
	shader->bindUniform("ModelMatrix", transform);
	//all other values are set by Scene, as they are the same for all objects
	
	mesh->draw();
}

glm::mat4 Instance::createTransform(const glm::vec3& position, const glm::vec3& eulerAngles, const glm::vec3& scale)
{
	//use glm functions to create transform matrix
	return glm::translate(glm::mat4(1), position) 
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.z), glm::vec3(0, 0, 1)) 
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.y), glm::vec3(0, 1, 0)) 
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.x), glm::vec3(1, 0, 0)) 
		* glm::scale(glm::mat4(1), scale);
}
