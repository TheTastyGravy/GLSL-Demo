/*  Created: 11/3/2021
 *  Author: Thomas Dufresne
 *
 *  Last Modified: 8/4/2021
 */
#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
	Camera(glm::vec3 a_position, bool a_isStatic = false, float a_phi = .0f, float a_theta = .0f);
	~Camera() {};

	void update(float a_deltaTime);
	glm::vec3 getPosition() const { return m_position; }

	// Matrix used to convert into the cameras local space
	glm::mat4 getViewMatrix() const;
	// Matrix used to project into clip space
	glm::mat4 getProjectionMatrix(float a_windowWidth, float a_windowHeight) const;
	// Matrix used to project into clip space
	glm::mat4 getProjectionMatrix(glm::vec2 a_windowSize) const { return getProjectionMatrix(a_windowSize.x, a_windowSize.y); }

	bool isCameraStatic() const { return m_isStatic; }
	
private:
	float m_theta;	//in degrees
	float m_phi;	//in degrees

	glm::vec3 m_position;
	// A static camera can not move or rotate
	bool m_isStatic;

	float m_lastMouseX, m_lastMouseY;
};