#include "Scene.h"
#include "Instance.h"
#include "Shader.h"
#include "Camera.h"
#include <string>
#include <Gizmos.h>
#include <glm/ext.hpp>


Scene::Scene(Camera* camera, glm::vec2 windowSize, glm::vec3 ambientLight)
{
	this->camera = camera;
	this->windowSize = windowSize;
	this->ambientLight = ambientLight;
}

Scene::~Scene()
{
	for (auto shader : shaders)
	{
		delete shader;
	}

	for (auto light : directionalLights)
	{
		delete light;
	}
	for (auto light : pointLights)
	{
		delete light;
	}
}

void Scene::addInstance(Instance* instance)
{
	instances.push_back(instance);


	//check if we have the instances shader yet
	bool hasShader = false;
	for (auto shader : shaders)
	{
		if (instance->getShader() == shader)
		{
			hasShader = true;
			break;
		}
	}

	//if not, add it
	if (!hasShader)
	{
		shaders.push_back(instance->getShader());
	}
}


void Scene::draw()
{
	//lambda function to bind lights in a shader
	auto bindLights = [this](aie::ShaderProgram* shader)
	{
		shader->bindUniform("AmbientColor", ambientLight);

		int directionalLightCount = directionalLights.size();
		shader->bindUniform("DirectionalLightCount", directionalLightCount);
		int pointLightCount = pointLights.size();
		shader->bindUniform("PointLightCount", pointLightCount);

		//add each light to the shader
		for (int i = 0; i < directionalLightCount; i++)
		{
			shader->bindUniform(("DirectionalLights[" + std::to_string(i) + "].Direction").c_str(), directionalLights[i]->direction);
			shader->bindUniform(("DirectionalLights[" + std::to_string(i) + "].Color").c_str(), directionalLights[i]->color);
		}
		for (int i = 0; i < pointLightCount; i++)
		{
			shader->bindUniform(("PointLights[" + std::to_string(i) + "].Position").c_str(), pointLights[i]->position);
			shader->bindUniform(("PointLights[" + std::to_string(i) + "].Color").c_str(), pointLights[i]->color);
		}
	};

	
	for (auto shader : shaders)
	{
		shader->bind();
		
		//bind camera position
		shader->bindUniform("CameraPosition", camera->getPosition());

		//bind lighting
		bindLights(shader);
	}

	for (auto instance : instances)
	{
		instance->draw(this);
	}
}
