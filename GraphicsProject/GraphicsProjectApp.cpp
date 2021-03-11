#include "GraphicsProjectApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


GraphicsProjectApp::GraphicsProjectApp()
{
	cameraIndex = 0;
}

GraphicsProjectApp::~GraphicsProjectApp()
{
}

bool GraphicsProjectApp::startup()
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	aie::Gizmos::create(10000, 10000, 10000, 10000);

	light.color = { 0.8f, 0.8f, 0.8f };
	ambientLight = { 0.25f, 0.25f, 0.25f };

	return loadShaderAndMeshLogic();
}

void GraphicsProjectApp::shutdown()
{
	aie::Gizmos::destroy();
}


void GraphicsProjectApp::update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();

	//update GUI tools
	IMGUI_Logic();

	//rotate light
	float time = getTime();
	light.direction = glm::normalize(glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));

	//update current camera
	camera[cameraIndex].update(deltaTime);
	//swap camera with numbers
	if (input->wasKeyPressed(aie::INPUT_KEY_1))
	{
		cameraIndex = 0;
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_2))
	{
		cameraIndex = 1;
	}
	

	// quit if we press escape
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
	{
		quit();
	}
	//rotate plane
	//if (input->isKeyDown(aie::INPUT_KEY_UP))
	//{
	//	quadTransform = glm::rotate(quadTransform, deltaTime * glm::radians(90.f), glm::vec3(1, 0, 0));
	//}
	//if (input->isKeyDown(aie::INPUT_KEY_DOWN))
	//{
	//	quadTransform = glm::rotate(quadTransform, -deltaTime * glm::radians(90.f), glm::vec3(1, 0, 0));
	//}
	//if (input->isKeyDown(aie::INPUT_KEY_LEFT))
	//{
	//	quadTransform = glm::rotate(quadTransform, -deltaTime * glm::radians(90.f), glm::vec3(0, 1, 0));
	//}
	//if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
	//{
	//	quadTransform = glm::rotate(quadTransform, deltaTime * glm::radians(90.f), glm::vec3(0, 1, 0));
	//}
}

void GraphicsProjectApp::draw()
{
	// wipe the screen to the background colour
	clearScreen();
	aie::Gizmos::clear();

	//get camera matrices
	glm::mat4 projectionMatrix = camera[cameraIndex].getProjectionMatrix(getWindowWidth(), getWindowHeight());
	glm::mat4 viewMatrix = camera[cameraIndex].getViewMatrix();


	// draw a simple grid with gizmos
	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i)
	{
		aie::Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
			glm::vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		aie::Gizmos::addLine(glm::vec3(10, 0, -10 + i),
			glm::vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	// ----- DRAW -----
	drawShaderAndMeshs(projectionMatrix, viewMatrix);

	aie::Gizmos::draw(projectionMatrix * viewMatrix);
}


bool GraphicsProjectApp::loadShaderAndMeshLogic()
{
	//load shaders
#pragma region Shaders
	//simple shader
	simpleShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simple.vert");
	simpleShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simple.frag");

	if (!simpleShader.link())
	{
		printf("Simple shader has an error: %s\n", simpleShader.getLastError());
		return false;
	}

	//phong shader
	phongShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
	phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");

	if (!phongShader.link())
	{
		printf("Phong shader has an error: %s", phongShader.getLastError());
		return false;
	}
#pragma endregion


#pragma region Quad
	//define the 6 verticies for the two triangles of a quad
	//Mesh::Vertex verticesNoIndex[6];
	//verticesNoIndex[0].position = { -0.5f, 0.f, 0.5f,  1.f };
	//verticesNoIndex[1].position = { 0.5f,  0.f, 0.5f,  1.f };
	//verticesNoIndex[2].position = { -0.5f, 0.f, -0.5f, 1.f };
	//
	//verticesNoIndex[3].position = { -0.5f, 0.f, -0.5f, 1.f };
	//verticesNoIndex[4].position = { 0.5f,  0.f, 0.5f,  1.f };
	//verticesNoIndex[5].position = { 0.5f,  0.f, -0.5f, 1.f };

	//use the corner vertices
	Mesh::Vertex vertices[4];
	vertices[0].position = { -0.5f, 0.f, 0.5f,  1.f };
	vertices[1].position = { 0.5f,  0.f, 0.5f,  1.f };
	vertices[2].position = { -0.5f, 0.f, -0.5f, 1.f };
	vertices[3].position = { 0.5f,  0.f, -0.5f, 1.f };
	//index the order to use vertices in
	unsigned int indices[6] = { 0, 1, 2, 2, 1, 3 };	//[012] [213]

	quadMesh.Initialise(4, vertices, 6, indices);


	//make the quad 10x10 units
	quadTransform =
	{
		10, 0, 0, 0,
		0, 10, 0, 0,
		0, 0, 10, 0,
		0, 0, 0, 1
	};
#pragma endregion

#pragma region Bunny
	//check if can load bunny mesh
	if (!bunny.mesh.load("./stanford/bunny.obj"))
	{
		printf("Bunny mesh failed\n");
		return false;
	}
	bunny.material = &bunny.mesh.getMaterial(0);

	bunny.transform =
	{
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		-3, 0, 2, 1
	};
#pragma endregion

#pragma region Buddha
	//check if can load buddha mesh
	if (!buddha.mesh.load("./stanford/buddha.obj"))
	{
		printf("Buddha mesh failed\n");
		return false;
	}
	buddha.material = &buddha.mesh.getMaterial(0);

	buddha.transform =
	{
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		-2, 0, -3, 1
	};
#pragma endregion

#pragma region Dragon
	//check if can load dragon mesh
	if (!dragon.mesh.load("./stanford/dragon.obj"))
	{
		printf("Dragon mesh failed\n");
		return false;
	}
	dragon.material = &dragon.mesh.getMaterial(0);

	dragon.transform =
	{
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		0, 0, 0, 1
	};
#pragma endregion

#pragma region Lucy
	//check if can load lucy mesh
	if (!lucy.mesh.load("./stanford/lucy.obj"))
	{
		printf("Lucy mesh failed\n");
		return false;
	}
	lucy.material = &lucy.mesh.getMaterial(0);

	lucy.transform =
	{
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		2, 0, -3, 1
	};
#pragma endregion

	return true;
}

void GraphicsProjectApp::drawOBJMesh(aie::ShaderProgram& shader, const GraphicsProjectApp::MeshObject& obj, const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
	//bind the PVM
	glm::mat4 pvm = projMatrix * viewMatrix * obj.transform;
	shader.bindUniform("ProjectionViewModel", pvm);
	//bind the lighting transforms
	shader.bindUniform("ModelMatrix", obj.transform);

	obj.mesh.draw();
}

void GraphicsProjectApp::drawShaderAndMeshs(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
#pragma region Quad
	simpleShader.bind();
	//bind mesh transform (projection view matrix)
	auto pvm = projectionMatrix * viewMatrix * quadTransform;
	simpleShader.bindUniform("ProjectionViewModel", pvm);

	quadMesh.draw();
#pragma endregion


	//setup phong shader
#pragma region Phong
	//bind the shader
	phongShader.bind();

	//bind camera position
	phongShader.bindUniform("CameraPosition", glm::vec3(glm::inverse(viewMatrix)[3]));

	//bind lighting
	phongShader.bindUniform("AmbientColor", ambientLight);
	phongShader.bindUniform("LightColor", light.color);
	phongShader.bindUniform("LightDirection", light.direction);
#pragma endregion

	//draw each object using the phong shader
	drawOBJMesh(phongShader, bunny, projectionMatrix, viewMatrix);
	drawOBJMesh(phongShader, buddha, projectionMatrix, viewMatrix);
	drawOBJMesh(phongShader, dragon, projectionMatrix, viewMatrix);
	drawOBJMesh(phongShader, lucy, projectionMatrix, viewMatrix);
}


void GraphicsProjectApp::IMGUI_Logic()
{
	ImGui::Begin("Tool");

	ImGui::Text("Lighting");
	ImGui::Indent(25.f);
	ImGui::DragFloat3("Sunlight Direction", &light.direction[0], 0.1f, -1.f, 1.f);
	ImGui::ColorEdit3("Sunlight Color", &light.color[0]);
	ImGui::Unindent(25.f);

	ImGui::Spacing();
	ImGui::Text("Bunny");
	ImGui::Indent(25.f);
	ImGui::DragFloat3("Bunny Position", &(bunny.transform[3])[0], 0.25f);
	ImGui::ColorEdit3("Bunny Ambient Color", &bunny.material->ambient[0]);
	ImGui::ColorEdit3("Bunny Diffuse Color", &bunny.material->diffuse[0]);
	ImGui::ColorEdit3("Bunny Specular Color", &bunny.material->specular[0]);
	ImGui::ColorEdit3("Bunny Emission Color", &bunny.material->emissive[0]);
	ImGui::SliderFloat("Bunny Visibility", &bunny.material->opacity, 0, 1);
	ImGui::DragFloat("Bunny Specular Power", &bunny.material->specularPower, 1, 1, 100);
	ImGui::Unindent(25.f);

	ImGui::Spacing();
	ImGui::Text("Buddha");
	ImGui::Indent(25.f);
	ImGui::DragFloat3("Buddha Position", &(buddha.transform[3])[0], 0.25f);
	ImGui::ColorEdit3("Buddha Ambient Color", &buddha.material->ambient[0]);
	ImGui::ColorEdit3("Buddha Diffuse Color", &buddha.material->diffuse[0]);
	ImGui::ColorEdit3("Buddha Specular Color", &buddha.material->specular[0]);
	ImGui::ColorEdit3("Buddha Emission Color", &buddha.material->emissive[0]);
	ImGui::SliderFloat("Buddha Visibility", &buddha.material->opacity, 0, 1);
	ImGui::DragFloat("Buddha Specular Power", &buddha.material->specularPower, 1, 1, 100);
	ImGui::Unindent(25.f);

	ImGui::Spacing();
	ImGui::Text("Dragon");
	ImGui::Indent(25.f);
	ImGui::DragFloat3("Dragon Position", &(dragon.transform[3])[0], 0.25f);
	ImGui::ColorEdit3("Dragon Ambient Color", &dragon.material->ambient[0]);
	ImGui::ColorEdit3("Dragon Diffuse Color", &dragon.material->diffuse[0]);
	ImGui::ColorEdit3("Dragon Specular Color", &dragon.material->specular[0]);
	ImGui::ColorEdit3("Dragon Emission Color", &dragon.material->emissive[0]);
	ImGui::SliderFloat("Dragon Visibility", &dragon.material->opacity, 0, 1);
	ImGui::DragFloat("Dragon Specular Power", &dragon.material->specularPower, 1, 1, 100);
	ImGui::Unindent(25.f);

	ImGui::Spacing();
	ImGui::Text("Lucy");
	ImGui::Indent(25.f);
	ImGui::DragFloat3("Lucy Position", &(lucy.transform[3])[0], 0.25f);
	ImGui::ColorEdit3("Lucy Ambient Color", &lucy.material->ambient[0]);
	ImGui::ColorEdit3("Lucy Diffuse Color", &lucy.material->diffuse[0]);
	ImGui::ColorEdit3("Lucy Specular Color", &lucy.material->specular[0]);
	ImGui::ColorEdit3("Lucy Emission Color", &lucy.material->emissive[0]);
	ImGui::SliderFloat("Lucy Visibility", &lucy.material->opacity, 0, 1);
	ImGui::DragFloat("Lucy Specular Power", &lucy.material->specularPower, 1, 1, 100);
	ImGui::Unindent(25.f);

	ImGui::End();
}
