#include "Camera.h"
#include <glm/ext.hpp>
#include <Input.h>


Camera::Camera(glm::vec3 a_position, bool a_isStatic, float a_phi, float a_theta)
{
	m_position = a_position;
	m_phi = a_phi;
	m_theta = a_theta;
	m_isStatic = a_isStatic;

	m_lastMouseX = 0;
	m_lastMouseY = 0;
}


void Camera::update(float a_deltaTime)
{
	//static cameras dont need to move
	if (isCameraStatic())
	{
		return;
	}


	aie::Input* input = aie::Input::getInstance();
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);

	//find the forward, right and up axes for the camera
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR), glm::cos(phiR) * glm::sin(thetaR));
	glm::vec3 right(-glm::sin(thetaR), 0, glm::cos(thetaR));
	glm::vec3 up(0, 1, 0);

#pragma region InputMovement
	float speedMult = (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT)) ? 8 : 3;

	if (input->isKeyDown(aie::INPUT_KEY_E))
	{
		m_position += up * a_deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_Q))
	{
		m_position -= up * a_deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_A))
	{
		m_position -= right * a_deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_D))
	{
		m_position += right * a_deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_W))
	{
		m_position += forward * a_deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_S))
	{
		m_position -= forward * a_deltaTime * speedMult;
	}
#pragma endregion

	//get the current position of the mouse
	float mX = input->getMouseX();
	float mY = input->getMouseY();
	constexpr float turnSpeed = glm::radians(360.f);


	//use mouse delta to rotate while holding M2
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		m_theta += turnSpeed * (mX - m_lastMouseX) * a_deltaTime;
		m_phi += turnSpeed * (mY - m_lastMouseY) * a_deltaTime;
	}

	//store the last values
	m_lastMouseX = mX;
	m_lastMouseY = mY;
}


glm::mat4 Camera::getViewMatrix() const
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	//find the forward vector
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR), glm::cos(phiR) * glm::sin(thetaR));

	return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProjectionMatrix(float a_windowWidth, float a_windowHeight) const
{
	return glm::perspective(glm::pi<float>() * 0.25f, a_windowWidth / a_windowHeight, 0.1f, 1000.f);
}
