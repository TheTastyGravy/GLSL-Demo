#include "GraphicsProjectApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Planet.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

GraphicsProjectApp::GraphicsProjectApp()
{
}

GraphicsProjectApp::~GraphicsProjectApp()
{
}

bool GraphicsProjectApp::startup()
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);


	//solarSystem();
	

	return loadShaderAndMeshLogic();
}

void GraphicsProjectApp::shutdown()
{
	Gizmos::destroy();
}

void GraphicsProjectApp::update(float deltaTime)
{
	//update planets
	for (auto planet : planets)
	{
		planet->update(deltaTime);
	}
	

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
	{
		quit();
	}
	
	//rotate plane
	if (input->isKeyDown(aie::INPUT_KEY_UP))
	{
		quadTransform = glm::rotate(quadTransform, deltaTime * glm::radians(90.f), glm::vec3(1, 0, 0));
	}
	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
	{
		quadTransform = glm::rotate(quadTransform, -deltaTime * glm::radians(90.f), glm::vec3(1, 0, 0));
	}
	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
	{
		quadTransform = glm::rotate(quadTransform, -deltaTime * glm::radians(90.f), glm::vec3(0, 1, 0));
	}
	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
	{
		quadTransform = glm::rotate(quadTransform, deltaTime * glm::radians(90.f), glm::vec3(0, 1, 0));
	}
}

void GraphicsProjectApp::draw()
{
	// wipe the screen to the background colour
	clearScreen();
	Gizmos::clear();

	// update perspective based on screen size
	projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// ----- DRAW -----

	drawShaderAndMeshs(projectionMatrix, viewMatrix);

	for (auto planet : planets)
	{
		planet->drawGizmos();
	}


	Gizmos::draw(projectionMatrix * viewMatrix);
}


void GraphicsProjectApp::solarSystem()
{
	float sizeMod = 0.2f;
	float distMod = 0.5f;
	float speedMod = 5.f;

	//create planets
	planets.push_back(new Planet(0.2439f * sizeMod, vec3(0), 0.57f * distMod, 0.240f * speedMod, 1));	//mercury
	planets.push_back(new Planet(0.6051f * sizeMod, vec3(0), 1.08f * distMod, 0.615f * speedMod, 1));	//venus
	planets.push_back(new Planet(0.6371f * sizeMod, vec3(0), 1.49f * distMod, 1.000f * speedMod, 1));	//earth
	planets.push_back(new Planet(0.3389f * sizeMod, vec3(0), 2.27f * distMod, 1.881f * speedMod, 1));	//mars

	planets.push_back(new Planet(6.9911f * sizeMod, vec3(0), 7.78f * distMod, 11.86f * speedMod, 1));	//jupiter
	planets.push_back(new Planet(5.8232f * sizeMod, vec3(0), 14.3f * distMod, 29.46f * speedMod, 1));	//saturn
	planets.push_back(new Planet(2.5362f * sizeMod, vec3(0), 28.7f * distMod, 84.01f * speedMod, 1));	//uranus
	planets.push_back(new Planet(2.4622f * sizeMod, vec3(0), 44.9f * distMod, 164.8f * speedMod, 1));	//neptune
}



bool GraphicsProjectApp::loadShaderAndMeshLogic()
{
	//load the vertex shader
	simpleShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simple.vert");

	//load the fragment shader
	simpleShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simple.frag");

	if (!simpleShader.link())
	{
		printf("Simple shader has an error: %s\n", simpleShader.getLastError());
		return false;
	}

	quadMesh.initialiseQuad();
	//make the quad 10x10 units
	quadTransform =
	{
		10, 0, 0, 0,
		0, 10, 0, 0,
		0, 0, 10, 0,
		0, 0, 0, 1
	};

	return true;
}

void GraphicsProjectApp::drawShaderAndMeshs(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	simpleShader.bind();

	//bind mesh transform (pvm = projection view matrix)
	auto pvm = projectionMatrix * viewMatrix * quadTransform;
	simpleShader.bindUniform("ProjectionViewModel", pvm);

	quadMesh.draw();
}
