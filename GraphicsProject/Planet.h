#pragma once
#include <glm/glm.hpp>

class Planet
{
public:
	Planet(float radius, glm::vec3 orbitCenter, float orbitRadius, float orbitSpeed, float rotationSpeed, glm::vec3 rotationAxis = glm::vec3(0, 1, 0), glm::vec4 color = glm::vec4(1, 0, 0, 1));

	void update(float deltaTime);
	void drawGizmos();

protected:
	float radius;

	glm::vec3 orbitCenter;
	float orbitRadius;
	float orbitSpeed;
	
	float rotationSpeed;
	glm::vec3 rotationAxis;

	glm::vec4 color;


	//position in world space
	glm::vec3 position;
	//used for rotation
	glm::mat4 transform;
};