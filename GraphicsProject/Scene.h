/*  Created: 18/3/2021
 *  Author: Thomas Dufresne
 *
 *  Last Modified: 8/4/2021
 *	
 *	Used to handle draw calls for meshes with custom shaders. Contains functionality for 
 *	lighting and cameras, with Instance acting as a container for rendered objects.
 */
#pragma once
#include <list>
#include <vector>
#include <glm/glm.hpp>

class Camera;
class Instance;
namespace aie
{
	class ShaderProgram;
}

struct DirectionalLight
{
	DirectionalLight(glm::vec3 a_direction = glm::vec3(0, -1, 0), glm::vec3 a_color = glm::vec3(1))
	{
		m_direction = a_direction;
		m_color = a_color;
	}

	glm::vec3 m_direction;
	glm::vec3 m_color;
};
struct PointLight
{
	PointLight(glm::vec3 a_position = glm::vec3(0), float a_range = 20, float a_brightness = 2, glm::vec3 a_color = glm::vec3(1))
	{
		m_position = a_position;
		m_range = a_range;
		m_brightness = a_brightness;
		m_color = a_color;
	}

	glm::vec3 m_position;
	float m_range;
	float m_brightness;
	glm::vec3 m_color;
};


class Scene
{
public:
	Scene(std::vector<Camera*> a_cameras, glm::vec2 a_windowSize, glm::vec3 a_ambientLight);
	~Scene();

	// Add an instance to the scene, registering its shader
	void addInstance(Instance* a_instance);
	// Add a directional light source to the scene
	void addLight(DirectionalLight* a_light) { m_directionalLights.push_back(a_light); }
	// Add a point light source to the scene
	void addLight(PointLight* a_light) { m_pointLights.push_back(a_light); }

	// Setup instance sahders and draw all instances in the scene
	void draw();


	std::vector<Camera*>& getCameras() { return m_cameras; }
	// Get the currently active camera
	Camera* getCurrentCamera() const { return m_cameras[m_cameraIndex]; }
	// Get the index of the current camera
	int getCameraIndex() const { return m_cameraIndex; }
	// Set the index of the camera to use
	void setCameraIndex(int a_index) { m_cameraIndex = a_index; }

	glm::vec2 getWindowSize() const { return m_windowSize; }

	glm::vec3& getAmbientLight() { return m_ambientLight; }
	std::vector<DirectionalLight*>& getDirectionalLights() { return m_directionalLights; }
	std::vector<PointLight*>& getPointLights() { return m_pointLights; }

protected:
	std::vector<Camera*> m_cameras;
	int m_cameraIndex = 0;
	glm::vec2 m_windowSize;

	///lights
	glm::vec3 m_ambientLight;
	std::vector<DirectionalLight*> m_directionalLights;
	std::vector<PointLight*> m_pointLights;
	
	std::list<Instance*> m_instances;
	// Each unique shader used by the scenes instances
	std::list<aie::ShaderProgram*> m_shaders;
};
