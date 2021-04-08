#include "Scene.h"
#include "Instance.h"
#include "Shader.h"
#include "Camera.h"
#include <string>
#include <Gizmos.h>
#include <glm/ext.hpp>


Scene::Scene(std::vector<Camera*> a_cameras, glm::vec2 a_windowSize, glm::vec3 a_ambientLight)
{
	m_cameras = a_cameras;
	m_windowSize = a_windowSize;
	m_ambientLight = a_ambientLight;
}

Scene::~Scene()
{
	for (auto cam : m_cameras)
	{
		delete cam;
	}
	for (auto shader : m_shaders)
	{
		delete shader;
	}

	for (auto light : m_directionalLights)
	{
		delete light;
	}
	for (auto light : m_pointLights)
	{
		delete light;
	}
}

void Scene::addInstance(Instance* a_instance)
{
	m_instances.push_back(a_instance);


	//check if we have the instances shader yet
	bool hasShader = false;
	for (auto shader : m_shaders)
	{
		if (a_instance->getShader() == shader)
		{
			hasShader = true;
			break;
		}
	}

	//if not, add it
	if (!hasShader)
	{
		m_shaders.push_back(a_instance->getShader());
	}
}


void Scene::draw()
{
	//lambda function to bind lights in a shader
	auto bindLights = [this](aie::ShaderProgram* a_shader)
	{
		a_shader->bindUniform("AmbientColor", m_ambientLight);

		//bind how many of each light there are
		int directionalLightCount = m_directionalLights.size();
		a_shader->bindUniform("DirectionalLightCount", directionalLightCount);
		int pointLightCount = m_pointLights.size();
		a_shader->bindUniform("PointLightCount", pointLightCount);

		//add each light to the shader
		for (int i = 0; i < directionalLightCount; i++)
		{
			a_shader->bindUniform(("DirectionalLights[" + std::to_string(i) + "].Direction").c_str(), m_directionalLights[i]->m_direction);
			a_shader->bindUniform(("DirectionalLights[" + std::to_string(i) + "].Color").c_str(), m_directionalLights[i]->m_color);
		}
		for (int i = 0; i < pointLightCount; i++)
		{
			a_shader->bindUniform(("PointLights[" + std::to_string(i) + "].Position").c_str(), m_pointLights[i]->m_position);
			a_shader->bindUniform(("PointLights[" + std::to_string(i) + "].Range").c_str(), m_pointLights[i]->m_range);
			a_shader->bindUniform(("PointLights[" + std::to_string(i) + "].Brightness").c_str(), m_pointLights[i]->m_brightness);
			a_shader->bindUniform(("PointLights[" + std::to_string(i) + "].Color").c_str(), m_pointLights[i]->m_color);
		}
	};

	//setup each shader before drawing
	for (auto shader : m_shaders)
	{
		shader->bind();
		
		//bind camera position, since all shaders use the same value
		shader->bindUniform("CameraPosition", m_cameras[m_cameraIndex]->getPosition());

		//bind lighting
		bindLights(shader);
	}
	//draw eah instance
	for (auto instance : m_instances)
	{
		//instances have their own shaders
		instance->draw(this);
	}
}
