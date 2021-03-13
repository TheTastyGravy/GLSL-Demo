#include "Camera.h"
#include <glm/ext.hpp>
#include <Input.h>


Camera::Camera()
{
	position = glm::vec3(-10, 2, 0);
	phi = 0;
	theta = 0;

	lastMouseX = 0;
	lastMouseY = 0;
}


void Camera::update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);

	//find the forward, right and up axes for the camera
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR), glm::cos(phiR) * glm::sin(thetaR));
	glm::vec3 right(-glm::sin(thetaR), 0, glm::cos(thetaR));
	glm::vec3 up(0, 1, 0);

#pragma region InputSettings
	float speedMult = (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT)) ? 8 : 3;

	if (input->isKeyDown(aie::INPUT_KEY_E))
	{
		position += up * deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_Q))
	{
		position -= up * deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_A))
	{
		position -= right * deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_D))
	{
		position += right * deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_W))
	{
		position += forward * deltaTime * speedMult;
	}
	if (input->isKeyDown(aie::INPUT_KEY_S))
	{
		position -= forward * deltaTime * speedMult;
	}
#pragma endregion

	//get the current position of the mouse
	float mX = input->getMouseX();
	float mY = input->getMouseY();
	const float turnSpeed = glm::radians(360.f);


	//if the right button is down, increment the theta and phi
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		theta += turnSpeed * (mX - lastMouseX) * deltaTime;
		phi += turnSpeed * (mY - lastMouseY) * deltaTime;
	}

	//store the last values
	lastMouseX = mX;
	lastMouseY = mY;
}


glm::mat4 Camera::getViewMatrix()
{
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR), glm::cos(phiR) * glm::sin(thetaR));

	return glm::lookAt(position, position + forward, glm::vec3(0, 1, 0));;
}

glm::mat4 Camera::getProjectionMatrix(float width, float height)
{
	//makes a pyramid
	return glm::perspective(glm::pi<float>() * 0.25f, width / height, 0.1f, 1000.f);
}
