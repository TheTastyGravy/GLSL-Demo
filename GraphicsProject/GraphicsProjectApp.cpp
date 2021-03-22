#include "GraphicsProjectApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Scene.h"
#include "Instance.h"
#include "Camera.h"
#include "Shader.h"
#include "ParticleGenerator.h"


GraphicsProjectApp::GraphicsProjectApp()
{
}

GraphicsProjectApp::~GraphicsProjectApp()
{
}

bool GraphicsProjectApp::startup()
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	//initialise gizmo primitive counts
	aie::Gizmos::create(10000, 10000, 10000, 10000);

	//create cameras (1 movable, 3 static)
	std::vector<Camera*> cams = std::vector<Camera*>();
	cams.push_back(new Camera(glm::vec3(-10, 3, 0)));
	cams.push_back(new Camera(glm::vec3(-10, 0, 0), true, 0, 0));
	cams.push_back(new Camera(glm::vec3(0, 15, 0), true, -90, 0));
	cams.push_back(new Camera(glm::vec3(0, 0, 10), true, 0, -90));

	//create new scene
	scene = new Scene(cams, glm::vec2(getWindowWidth(), getWindowHeight()), glm::vec3(.5f));
	//add lights
	scene->addLight(new DirectionalLight(glm::vec3(1, 0, 0), glm::vec3(1)));
	scene->addLight(new DirectionalLight(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
	scene->addLight(new PointLight(glm::vec3(0), 30, 2, glm::vec3(1, 0, 0)));

	particleGen = new ParticleGenerator(glm::vec3(0), scene, 50);
	particleGen->setup(3, 3, glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 0), glm::vec3(0, -1, 0), 2, 1, 0);
	
	//create mesh objects
	return loadShaderAndMeshLogic();
}

void GraphicsProjectApp::shutdown()
{
	aie::Gizmos::destroy();
	delete scene;

	delete particleGen;
}


void GraphicsProjectApp::update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();

	//update GUI tools
	IMGUI_Logic();

	//rotate light
	float time = getTime();
	scene->getDirectionalLights()[0]->direction = glm::normalize(glm::vec3(glm::cos(time * 2), 0, glm::sin(time * 2)));

	//update current camera
	scene->getCurrentCamera()->update(deltaTime);

	particleGen->update(deltaTime);

	//quit on escape
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
	{
		quit();
	}
}

void GraphicsProjectApp::draw()
{
	//clear the screen
	clearScreen();
	aie::Gizmos::clear();

	//get camera matrices
	glm::mat4 projectionMatrix = scene->getCurrentCamera()->getProjectionMatrix(getWindowWidth(), getWindowHeight());
	glm::mat4 viewMatrix = scene->getCurrentCamera()->getViewMatrix();


	//draw gizmo for point lights
	for (auto pointLight : scene->getPointLights())
	{
		aie::Gizmos::addSphere(pointLight->position, 0.2f, 6, 6, glm::vec4(pointLight->color, 1));
	}

	//draw
	scene->draw();
	particleGen->draw();

	aie::Gizmos::draw(projectionMatrix * viewMatrix);
}


bool GraphicsProjectApp::loadShaderAndMeshLogic()
{
#pragma region Meshes
	//load bunny mesh
	if (!bunny.mesh.load("./stanford/bunny.obj"))
	{
		printf("Bunny mesh failed\n");
		return false;
	}
	bunny.material = &bunny.mesh.getMaterial(0);
	//load soul spear mesh
	if (!soulSpear.mesh.load("./soulspear/soulspear.obj", true, true))
	{
		printf("SoulSpear mesh failed\n");
		return false;
	}
	soulSpear.material = &soulSpear.mesh.getMaterial(0);
	//load m1 carbine mesh
	if (!m1Carbine.mesh.load("./M1_carbine/M1_carbine.obj", true, true))
	{
		printf("M1Carbine mesh failed\n");
		return false;
	}
	m1Carbine.material = &m1Carbine.mesh.getMaterial(0);
#pragma endregion

#pragma region Shaders
	//phong shader
	aie::ShaderProgram* phongShader = new aie::ShaderProgram();
	phongShader->loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
	phongShader->loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");
	if (!phongShader->link())
	{
		printf("Phong shader has an error: %s\n", phongShader->getLastError());
		return false;
	}
	//normal map shader
	aie::ShaderProgram* normalShader = new aie::ShaderProgram();
	normalShader->loadShader(aie::eShaderStage::VERTEX, "./shaders/normalMap.vert");
	normalShader->loadShader(aie::eShaderStage::FRAGMENT, "./shaders/normalMap.frag");
	if (!normalShader->link())
	{
		printf("Normal map shader has an error: %s\n", normalShader->getLastError());
		return false;
	}
#pragma endregion

	Instance* instance;
	//add soul spears
	for (int i = 0; i < 10; i++)
	{
		instance = new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, i * 30, 0), glm::vec3(1), &soulSpear.mesh, normalShader);
		transforms.push_back({ "Soul Spear " + std::to_string(i), instance->getTransform(),
								glm::vec3(i * 2, 0, 0), glm::vec3(0, i * 30, 0), glm::vec3(1) });
		scene->addInstance(instance);
	}

	//add m1carbine
	instance = new Instance(glm::vec3(-2, 0, 0), glm::vec3(0, 90, 0), glm::vec3(.1f), &m1Carbine.mesh, normalShader);
	transforms.push_back({ "M1 Carbine", instance->getTransform(), 
							glm::vec3(-2, 0, 0), glm::vec3(0, 90, 0), glm::vec3(.1f) });
	scene->addInstance(instance);
	//add bunny
	instance = new Instance(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(.2f), &bunny.mesh, phongShader);
	transforms.push_back({ "Bunny", instance->getTransform(),
							glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(.2f) });
	scene->addInstance(instance);


	return true;
}


void GraphicsProjectApp::IMGUI_Logic()
{
#pragma region Lighting Editor
	ImGui::Begin("Lighting Editor");
	//ambient light
	ImGui::ColorEdit3("Ambient Light Color", &scene->getAmbientLight()[0]);
	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();


	//	----- DIRECTIONAL LIGHT -----
	std::vector<DirectionalLight*>& directionalLights = scene->getDirectionalLights();
	static int currentDirLight = 0;
	//display light count
	ImGui::Text(("Number of directional lights: " + std::to_string(directionalLights.size())).c_str());
	//buttons to create and delete lights
	if (ImGui::Button("New Dir Light", ImVec2(150, 20)))
	{
		//add light and set editor to it
		directionalLights.push_back(new DirectionalLight(glm::vec3(1), glm::vec3(0)));
		currentDirLight = directionalLights.size() - 1;
	}
	if (currentDirLight != 0)
	{
		//delete button
		if (ImGui::Button("Delete Current Dir Light", ImVec2(220, 20)))
		{
			directionalLights.erase(directionalLights.begin() + currentDirLight);
			//if the last light was removed, move down
			if (currentDirLight == directionalLights.size())
			{
				currentDirLight--;
			}
		}
	}

	//light editer
	ImGui::Text(("Current Dir Light: " + std::to_string(currentDirLight + 1)).c_str());
	ImGui::DragFloat3("Dir Light Direction", &directionalLights[currentDirLight]->direction[0], 0.1f, -1.f, 1.f);
	ImGui::ColorEdit3("Dir Light Color", &directionalLights[currentDirLight]->color[0]);
	//nav buttons
	if (currentDirLight != 0)
	{
		if (ImGui::Button("Prev Dir", ImVec2(100, 20)))
		{
			currentDirLight--;
		}
		//next button should be on the same line
		ImGui::SameLine(0, 0);
	}
	if (currentDirLight != directionalLights.size() - 1)
	{
		//move over from the prev button
		ImGui::Indent(110);
		if (ImGui::Button("Next Dir", ImVec2(100, 20)))
		{
			currentDirLight++;
		}
		ImGui::Unindent(110);
	}


	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

	//	----- POINT LIGHT -----
	std::vector<PointLight*>& pointLights = scene->getPointLights();
	static int currentPointLight = 0;
	//display light count
	ImGui::Text(("Number of point lights: " + std::to_string(pointLights.size())).c_str());
	//buttons to create and delete lights
	if (ImGui::Button("New Point Light", ImVec2(150, 20)))
	{
		//add light and set editor to it
		pointLights.push_back(new PointLight());
		currentPointLight = pointLights.size() - 1;
	}
	if (currentPointLight != 0)
	{
		//delete button
		if (ImGui::Button("Delete Current Point Light", ImVec2(220, 20)))
		{
			pointLights.erase(pointLights.begin() + currentPointLight);
			//if the last light was removed, move down
			if (currentPointLight == pointLights.size())
			{
				currentPointLight--;
			}
		}
	}

	//light editor
	ImGui::Text(("Current Point Light: " + std::to_string(currentPointLight + 1)).c_str());
	ImGui::DragFloat3("Point Light Position", &pointLights[currentPointLight]->position[0], 0.25f);
	ImGui::DragFloat("Point Light Range", &pointLights[currentPointLight]->range, 0.1f, 0, 100);
	ImGui::DragFloat("Point Light Brightness", &pointLights[currentPointLight]->brightness, 0.1f, 0, 100);
	ImGui::ColorEdit3("Point Light Color", &pointLights[currentPointLight]->color[0]);
	//nav buttons
	if (currentPointLight != 0)
	{
		if (ImGui::Button("Prev Point", ImVec2(100, 20)))
		{
			currentPointLight--;
		}
		//next button should be on the same line
		ImGui::SameLine(0, 0);
	}
	if (currentPointLight != pointLights.size() - 1)
	{
		//move over from the prev button
		ImGui::Indent(110);
		if (ImGui::Button("Next Point", ImVec2(100, 20)))
		{
			currentPointLight++;
		}
		ImGui::Unindent(110);
	}

	ImGui::End();
#pragma endregion

	//create tools for editing materials
	ImGui::Begin("Material Editor");
	imguiMaterialTool("Bunny", bunny);
	imguiMaterialTool("Soul Spear", soulSpear);
	imguiMaterialTool("M1 Carbine", m1Carbine);
	ImGui::End();


	//create tools for editing each objects transform
	ImGui::Begin("Transform Editor");
	static int currentTransform = 0;
	EditorTransform& trans = transforms[currentTransform];
	
	ImGui::Text(trans.name.c_str());
	//editor for transform
	ImGui::DragFloat3("Position", &trans.position[0], 0.25f);
	ImGui::DragFloat3("Rotation", &trans.rotation[0], 0.25f);
	ImGui::DragFloat3("Scale", &trans.scale[0], 0.25f);
	//nav buttons
	if (ImGui::Button("Prev"))
	{
		currentTransform--;
		if (currentTransform < 0)
		{
			currentTransform = transforms.size() - 1;
		}
	}
	ImGui::SameLine(0, 0);
	ImGui::Indent(50);
	if (ImGui::Button("Next"))
	{
		currentTransform++;
		if (currentTransform > transforms.size() + 1)
		{
			currentTransform = 0;
		}
	}

	//update transform with new pos, rot, and scale
	trans.transform = Instance::createTransform(trans.position, trans.rotation, trans.scale);
	ImGui::End();


	//create tool for swapping camera
	ImGui::Begin("Camera Controller");
	int camIndex = scene->getCameraIndex();
	//display current camera and weather its static or movable
	ImGui::Text(("Current camera: " + std::to_string(camIndex)).c_str());
	ImGui::Text((std::string("Camera is ") + (scene->getCurrentCamera()->isCameraStatic() ? "static" : "movable")).c_str());

	//nav buttons
	if (camIndex > 0)
	{
		if (ImGui::Button("Prev"))
		{
			scene->setCameraIndex(camIndex - 1);
		}
		ImGui::SameLine(0, 0);
	}
	if (camIndex < scene->getCameras().size() - 1)
	{
		ImGui::Indent(50);
		if (ImGui::Button("Next"))
		{
			scene->setCameraIndex(camIndex + 1);
		}
	}
	ImGui::End();
}

// Create ImGui components to edit 'obj'
void GraphicsProjectApp::imguiMaterialTool(std::string name, MeshObject& obj)
{
	ImGui::Spacing();
	ImGui::Text(name.c_str());
	ImGui::Indent(25.f);
	ImGui::ColorEdit3((name + " Ambient Color").c_str(), &obj.material->ambient[0]);
	ImGui::ColorEdit3((name + " Diffuse Color").c_str(), &obj.material->diffuse[0]);
	ImGui::ColorEdit3((name + " Specular Color").c_str(), &obj.material->specular[0]);
	ImGui::ColorEdit3((name + " Emission Color").c_str(), &obj.material->emissive[0]);
	ImGui::SliderFloat((name + " Visibility").c_str(), &obj.material->opacity, 0, 1);
	ImGui::DragFloat((name + " Specular Power").c_str(), &obj.material->specularPower, 1, 1, 100);
	ImGui::Unindent(25.f);
}