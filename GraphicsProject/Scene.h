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
	PointLight(glm::vec3 position = glm::vec3(0), glm::vec3 color = glm::vec3(1))
	{
		this->position = position;
		this->color = color;
	}

	glm::vec3 position;
	glm::vec3 color;
};


class Scene
{
public:
	Scene(std::vector<Camera*> cameras, glm::vec2 windowSize, glm::vec3 ambientLight);
	~Scene();

	void addInstance(Instance* instance);
	void addLight(DirectionalLight* light) { directionalLights.push_back(light); }
	void addLight(PointLight* light) { pointLights.push_back(light); }

	void draw();


	std::vector<Camera*>& getCameras() { return cameras; }
	Camera* getCurrentCamera() const { return cameras[cameraIndex]; }
	int getCameraIndex() const { return cameraIndex; }
	void setCameraIndex(int index) { cameraIndex = index; }

	glm::vec2 getWindowSize() const { return windowSize; }

	glm::vec3& getAmbientLight() { return ambientLight; }
	std::vector<DirectionalLight*>& getDirectionalLights() { return directionalLights; }
	std::vector<PointLight*>& getPointLights() { return pointLights; }

protected:
	std::vector<Camera*> cameras;
	int cameraIndex = 0;
	glm::vec2 windowSize;

	//lights
	glm::vec3 ambientLight;
	std::vector<DirectionalLight*> directionalLights;
	std::vector<PointLight*> pointLights;
	
	std::list<Instance*> instances;
	std::list<aie::ShaderProgram*> shaders;

};
