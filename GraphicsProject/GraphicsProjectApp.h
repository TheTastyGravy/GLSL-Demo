#pragma once
#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "OBJMesh.h"
#include "Camera.h"
#include "Scene.h"


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
	
	void IMGUI_Logic();

protected:
	struct MeshObject
	{
		aie::OBJMesh mesh;
		aie::OBJMesh::Material* material;
	};
	// Used to edit objeects transforms with ImGui
	struct Transform
	{
		std::string name;
		glm::mat4& transform;
	};

	void imguiMaterialTool(std::string name, MeshObject& obj);


	Scene* scene;
	Camera* camera;

	//mesh objects
	MeshObject bunny;
	MeshObject soulSpear;
	MeshObject m1Carbine;

	std::vector<Transform> transforms;
	
};