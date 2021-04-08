/*  Created: 10/3/2021
 *  Author: Thomas Dufresne
 *
 *  Last Modified: 8/4/2021
 *
 *	Used as the entry point for the graphics application
 */
#pragma once
#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "OBJMesh.h"

class Scene;
class ParticleGenerator;


class GraphicsProjectApp : public aie::Application
{
public:
	GraphicsProjectApp();
	virtual ~GraphicsProjectApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float a_deltaTime);
	virtual void draw();

	bool loadShaderAndMeshLogic();
	
	void IMGUI_Logic();

protected:
	struct MeshObject
	{
		aie::OBJMesh m_mesh;
		aie::OBJMesh::Material* m_material;
	};
	// Used to edit objeects transforms with ImGui
	struct EditorTransform
	{
		std::string m_name;
		glm::mat4& m_transform;

		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;
	};

	// Create ImGui components to edit a mesh object
	void imguiMaterialTool(std::string a_name, MeshObject& a_obj);


	Scene* m_scene;

	ParticleGenerator* m_particleGen;

	//mesh objects
	MeshObject m_bunny;
	MeshObject m_soulSpear;
	MeshObject m_m1Carbine;

	std::vector<EditorTransform> m_transforms;
};