#include "Planet.h"
#include "Gizmos.h"
#include <glm/ext.hpp>


Planet::Planet(float radius, glm::vec3 orbitCenter, float orbitRadius, float orbitSpeed, float rotationSpeed, glm::vec3 rotationAxis, glm::vec4 color)
{
	this->radius = radius;

	this->orbitCenter = orbitCenter;
	this->orbitRadius = orbitRadius;
	this->orbitSpeed = glm::radians(orbitSpeed);

	this->rotationSpeed = rotationSpeed;
	this->rotationAxis = glm::normalize(rotationAxis);

	this->color = color;

	this->position = glm::vec3(orbitRadius, 0, 0);
	this->transform = (glm::mat4)glm::translate(glm::mat4(1.f), glm::vec3(0));
}


void Planet::update(float deltaTime)
{
	float orbitRotDif = orbitSpeed * deltaTime;

	//find new position in orbit
	glm::vec3 newPos;
	newPos.x = orbitCenter.x + (cos(orbitRotDif) * position.x) + (-sin(orbitRotDif) * position.z);
	newPos.y = orbitCenter.y + position.y;
	newPos.z = orbitCenter.z + (sin(orbitRotDif) * position.x) + (cos(orbitRotDif) * position.z);
	position = newPos;


	//rotate on axis
	transform = glm::rotate(transform, rotationSpeed * deltaTime, rotationAxis);
}

void Planet::drawGizmos()
{
	//draw orbit ring
	aie::Gizmos::addRing(glm::vec3(0), orbitRadius - 0.025f, orbitRadius + 0.025f, 20, glm::vec4(1));

	//draw line for rotation axis
	aie::Gizmos::addLine(position - rotationAxis * radius * 1.5f, position + rotationAxis * radius * 1.5f, glm::vec4(0, 1, 0, 1));

	//draw planet
	aie::Gizmos::addSphere(position, radius, 10, 10, color, &transform);
}
