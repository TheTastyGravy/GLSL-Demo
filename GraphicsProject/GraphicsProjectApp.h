#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>;
#include "Mesh.h"
#include "OBJMesh.h"
#include "Shader.h"
#include "Camera.h"

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
	
	void IMGUI_Logic();

protected:
	struct MeshObject
	{
		aie::OBJMesh mesh;
		glm::mat4 transform;
		aie::OBJMesh::Material* material;
	};
	void drawOBJMesh(aie::ShaderProgram& shader, const MeshObject& obj, const glm::mat4& projViewMatrix);

	void imguiObjectTool(std::string name, MeshObject& obj);


	Camera camera[2];
	//which camera is active?
	unsigned int cameraIndex;


	//shaders
	aie::ShaderProgram phongShader;
	aie::ShaderProgram textureShader;
	aie::ShaderProgram normalMapShader;

	//square
	Mesh quadMesh;
	glm::mat4 quadTransform;
	aie::Texture gridTexture;
	//mesh objects
	MeshObject bunny;
	MeshObject buddha;
	MeshObject dragon;
	MeshObject lucy;
	MeshObject soulSpear;
	MeshObject M1Carbine;
	MeshObject Gun;


	struct DirectionalLight
	{
		glm::vec3 direction;
		glm::vec3 color;
	};
	struct PointLight
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	glm::vec3 ambientLight;
};