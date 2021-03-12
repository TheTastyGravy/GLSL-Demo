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
	//phong shader
	phongShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
	phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");
	if (!phongShader.link())
	{
		printf("Phong shader has an error: %s\n", phongShader.getLastError());
		return false;
	}

	//texture shader
	textureShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/textured.vert");
	textureShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/textured.frag");
	if (!textureShader.link())
	{
		printf("Texture shader has an error: %s\n", textureShader.getLastError());
		return false;
	}

	//normal map shader
	normalMapShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/normalMap.vert");
	normalMapShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/normalMap.frag");
	if (!normalMapShader.link())
	{
		printf("Normal map shader has an error: %s\n", normalMapShader.getLastError());
		return false;
	}
#pragma endregion


#pragma region Quad
	quadMesh.initialiseQuad();

	//load texture
	if (!gridTexture.load("./textures/numbered_grid.tga"))
	{
		printf("Failed to load: numbered_grid.tga\n");
		return false;
	}

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

#pragma region SoulSpear
	//check if can load lucy mesh
	if (!soulSpear.mesh.load("./soulspear/soulspear.obj"))
	{
		printf("SoulSpear mesh failed\n");
		return false;
	}
	soulSpear.material = &soulSpear.mesh.getMaterial(0);

	soulSpear.transform =
	{
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		3, 0, 3, 1
	};
#pragma endregion

	return true;
}

void GraphicsProjectApp::drawOBJMesh(aie::ShaderProgram& shader, const GraphicsProjectApp::MeshObject& obj, const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
	shader.bind();

	//bind the PVM
	glm::mat4 pvm = projMatrix * viewMatrix * obj.transform;
	shader.bindUniform("ProjectionViewModel", pvm);
	//bind the lighting transforms
	shader.bindUniform("ModelMatrix", obj.transform);

	obj.mesh.draw();
}

void GraphicsProjectApp::drawShaderAndMeshs(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	//setup shaders
#pragma region PhongShader
	phongShader.bind();
	//bind camera position
	phongShader.bindUniform("CameraPosition", glm::vec3(glm::inverse(viewMatrix)[3]));

	//bind lighting
	phongShader.bindUniform("AmbientColor", ambientLight);
	phongShader.bindUniform("LightColor", light.color);
	phongShader.bindUniform("LightDirection", light.direction);
#pragma endregion
#pragma region NormalMapShader
	normalMapShader.bind();
	//bind camera position
	normalMapShader.bindUniform("CameraPosition", glm::vec3(glm::inverse(viewMatrix)[3]));

	//bind lighting
	normalMapShader.bindUniform("AmbientColor", ambientLight);
	normalMapShader.bindUniform("LightColor", light.color);
	normalMapShader.bindUniform("LightDirection", light.direction);
#pragma endregion


#pragma region Quad
	textureShader.bind();

	//bind mesh transform (projection view matrix)
	auto pvm = projectionMatrix * viewMatrix * quadTransform;
	textureShader.bindUniform("ProjectionViewModel", pvm);

	//bind the texture to location 0
	textureShader.bindUniform("diffuseTexture", 0);
	//bind the texture to the location
	gridTexture.bind(0);

	quadMesh.draw();
#pragma endregion

	//draw each object
	drawOBJMesh(phongShader, bunny, projectionMatrix, viewMatrix);
	drawOBJMesh(phongShader, buddha, projectionMatrix, viewMatrix);
	drawOBJMesh(phongShader, dragon, projectionMatrix, viewMatrix);
	drawOBJMesh(phongShader, lucy, projectionMatrix, viewMatrix);
	drawOBJMesh(normalMapShader, soulSpear, projectionMatrix, viewMatrix);
}


void GraphicsProjectApp::IMGUI_Logic()
{
	ImGui::Begin("Editor");

	ImGui::Text("Lighting");
	ImGui::Indent(25.f);
	ImGui::DragFloat3("Sunlight Direction", &light.direction[0], 0.1f, -1.f, 1.f);
	ImGui::ColorEdit3("Sunlight Color", &light.color[0]);
	ImGui::Unindent(25.f);

	//create tools for editing mesh objects
	imguiObjectTool("Bunny", bunny);
	imguiObjectTool("Buddha", buddha);
	imguiObjectTool("Dragon", dragon);
	imguiObjectTool("Lucy", lucy);
	imguiObjectTool("Soul Spear", soulSpear);

	ImGui::End();
}

void GraphicsProjectApp::imguiObjectTool(std::string name, MeshObject& obj)
{
	ImGui::Spacing();
	ImGui::Text(name.c_str());
	ImGui::Indent(25.f);
	ImGui::DragFloat3((name + " Position").c_str(), &(obj.transform[3])[0], 0.25f);
	ImGui::ColorEdit3((name + " Ambient Color").c_str(), &obj.material->ambient[0]);
	ImGui::ColorEdit3((name + " Diffuse Color").c_str(), &obj.material->diffuse[0]);
	ImGui::ColorEdit3((name + " Specular Color").c_str(), &obj.material->specular[0]);
	ImGui::ColorEdit3((name + " Emission Color").c_str(), &obj.material->emissive[0]);
	ImGui::SliderFloat((name + " Visibility").c_str(), &obj.material->opacity, 0, 1);
	ImGui::DragFloat((name + " Specular Power").c_str(), &obj.material->specularPower, 1, 1, 100);
	ImGui::Unindent(25.f);
}