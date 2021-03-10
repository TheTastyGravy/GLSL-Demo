#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "Mesh.h"
#include "OBJMesh.h"
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

	//shaders
	aie::ShaderProgram simpleShader;
	aie::ShaderProgram bunnyShader;
	aie::ShaderProgram statueShader;
	aie::ShaderProgram dragonShader;
	//square
	Mesh quadMesh;
	glm::mat4 quadTransform;
	//bunny
	aie::OBJMesh bunnyMesh;
	glm::mat4 bunnyTransform;
	//statue
	aie::OBJMesh statueMesh;
	glm::mat4 statueTransform;
	//dragon
	aie::OBJMesh dragonMesh;
	glm::mat4 dragonTransform;


	std::vector<Planet*> planets = std::vector<Planet*>();

};