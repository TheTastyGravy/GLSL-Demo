#include "Scene.h"
#include "Instance.h"
#include "Shader.h"
#include "Camera.h"
#include <string>
#include <Gizmos.h>
#include <glm/ext.hpp>


Scene::Scene(std::vector<Camera*> cameras, glm::vec2 windowSize, glm::vec3 ambientLight)
{
	this->cameras = cameras;
	this->windowSize = windowSize;
	this->ambientLight = ambientLight;
}

Scene::~Scene()
{
	for (auto cam : cameras)
	{
		delete cam;
	}
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

		//bind how many of each light there are
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
			shader->bindUniform(("PointLights[" + std::to_string(i) + "].Range").c_str(), pointLights[i]->range);
			shader->bindUniform(("PointLights[" + std::to_string(i) + "].Brightness").c_str(), pointLights[i]->brightness);
			shader->bindUniform(("PointLights[" + std::to_string(i) + "].Color").c_str(), pointLights[i]->color);
		}
	};

	//setup each shader before drawing
	for (auto shader : shaders)
	{
		shader->bind();
		
		//bind camera position, since all shaders use the same value
		shader->bindUniform("CameraPosition", cameras[cameraIndex]->getPosition());

		//bind lighting
		bindLights(shader);
	}
	//draw eah instance
	for (auto instance : instances)
	{
		//instances have their own shaders
		instance->draw(this);
	}
}
