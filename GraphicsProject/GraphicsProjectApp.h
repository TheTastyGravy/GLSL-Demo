#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Mesh.h"
#include "OBJMesh.h"
#include "Shader.h"

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
	void drawOBJMesh(aie::ShaderProgram& shader, const MeshObject& obj, const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

	//camera transforms
	glm::mat4	viewMatrix;
	glm::mat4	projectionMatrix;

	//shaders
	aie::ShaderProgram simpleShader;
	aie::ShaderProgram phongShader;

	//square
	Mesh quadMesh;
	glm::mat4 quadTransform;
	//mesh objects
	MeshObject bunny;
	MeshObject buddha;
	MeshObject dragon;
	MeshObject lucy;


	struct Light
	{
		glm::vec3 direction;
		glm::vec3 color;
	};

	Light light;
	glm::vec3 ambientLight;

};