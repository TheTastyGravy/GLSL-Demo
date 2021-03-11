#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
	Camera();
	~Camera() {};

	void update(float deltaTime);
	glm::vec3 getPosition() { return position; }

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix(float width, float height);
	
private:
	float theta;	//in degrees
	float phi;		//in degrees
	glm::vec3 position;

	float lastMouseX, lastMouseY;
};