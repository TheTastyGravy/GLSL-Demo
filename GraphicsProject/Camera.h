#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
	Camera(glm::vec3 position, bool isStatic = false, float phi = .0f, float theta = .0f);
	~Camera() {};

	void update(float deltaTime);
	glm::vec3 getPosition() const { return position; }

	// Matrix used to convert into the cameras local space
	glm::mat4 getViewMatrix() const;
	// Matrix used to project into clip space
	glm::mat4 getProjectionMatrix(float windowWidth, float windowHeight) const;
	// Matrix used to project into clip space
	glm::mat4 getProjectionMatrix(glm::vec2 windowSize) const { return getProjectionMatrix(windowSize.x, windowSize.y); }

	bool isCameraStatic() const { return isStatic; }
	
private:
	float theta;	//in degrees
	float phi;		//in degrees

	glm::vec3 position;
	// A static camera can not move or rotate
	bool isStatic;

	float lastMouseX, lastMouseY;
};