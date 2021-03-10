#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "Mesh.h"
#include "Shader.h"

class Planet;

class GraphicsProjectApp : public aie::Application
{
public:
	GraphicsProjectApp();
	virtual ~GraphicsProjectApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	bool loadShaderAndMeshLogic();
	void drawShaderAndMeshs(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

protected:
	void solarSystem();

	// camera transforms
	glm::mat4	viewMatrix;
	glm::mat4	projectionMatrix;

	//shader
	aie::ShaderProgram simpleShader;
	Mesh quadMesh;
	glm::mat4 quadTransform;


	std::vector<Planet*> planets = std::vector<Planet*>();

};