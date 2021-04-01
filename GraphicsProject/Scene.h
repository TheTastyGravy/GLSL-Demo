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
	DirectionalLight(glm::vec3 direction = glm::vec3(0, -1, 0), glm::vec3 color = glm::vec3(1))
	{
		this->direction = direction;
		this->color = color;
	}

	glm::vec3 direction;
	glm::vec3 color;
};
struct PointLight
{
	PointLight(glm::vec3 position = glm::vec3(0), float range = 20, float brightness = 2, glm::vec3 color = glm::vec3(1))
	{
		this->position = position;
		this->range = range;
		this->brightness = brightness;
		this->color = color;
	}

	glm::vec3 position;
	float range;
	float brightness;
	glm::vec3 color;
};


class Scene
{
public:
	Scene(std::vector<Camera*> cameras, glm::vec2 windowSize, glm::vec3 ambientLight);
	~Scene();

	// Add an instance to the scene, registering its shader
	void addInstance(Instance* instance);
	// Add a directional light source to the scene
	void addLight(DirectionalLight* light) { directionalLights.push_back(light); }
	// Add a point light source to the scene
	void addLight(PointLight* light) { pointLights.push_back(light); }

	// Setup instance sahders and draw all instances in the scene
	void draw();


	std::vector<Camera*>& getCameras() { return cameras; }
	// Get the currently active camera
	Camera* getCurrentCamera() const { return cameras[cameraIndex]; }
	// Get the index of the current camera
	int getCameraIndex() const { return cameraIndex; }
	// Set the index of the camera to use
	void setCameraIndex(int index) { cameraIndex = index; }

	glm::vec2 getWindowSize() const { return windowSize; }

	glm::vec3& getAmbientLight() { return ambientLight; }
	std::vector<DirectionalLight*>& getDirectionalLights() { return directionalLights; }
	std::vector<PointLight*>& getPointLights() { return pointLights; }

protected:
	std::vector<Camera*> cameras;
	int cameraIndex = 0;
	glm::vec2 windowSize;

	///lights
	glm::vec3 ambientLight;
	std::vector<DirectionalLight*> directionalLights;
	std::vector<PointLight*> pointLights;
	
	std::list<Instance*> instances;
	// Each unique shader used by the scenes instances
	std::list<aie::ShaderProgram*> shaders;
};
