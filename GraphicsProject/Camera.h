#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
	Camera(glm::vec3 position, bool isStatic = false, float phi = .0f, float theta = .0f);
	~Camera() {};

	void update(float deltaTime);
	glm::vec3 getPosition() { return position; }

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix(float width, float height);

	bool isCameraStatic() const { return isStatic; }
	
private:
	float theta;	//in degrees
	float phi;		//in degrees
	glm::vec3 position;
	//is the camera stationary, or able to move?
	bool isStatic;

	float lastMouseX, lastMouseY;
};