/*  Created: 18/3/2021
 *  Author: Thomas Dufresne
 *
 *  Last Modified: 8/4/2021
 *	
 *	Used by Scene to contain meshes and their shaders
 */
#pragma once
#include <glm/glm.hpp>

class Camera;
class Scene;

namespace aie
{
	class OBJMesh;
	class ShaderProgram;
}


class Instance
{
public:
	Instance(glm::mat4 a_transform, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader);
	// Uses a transform using position, eulerAngles, and scale
	Instance(const glm::vec3& a_position, const glm::vec3& a_eulerAngles, const glm::vec3& a_scale, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader);

	// Draw the instance's mesh using the scenes active camera
	// Assumes the shader's lighting and camera position are bound
	void draw(Scene* a_scene);

	// Create a transform with a set position, rotation, and scale
	static glm::mat4 createTransform(const glm::vec3& a_position, const glm::vec3& a_eulerAngles, const glm::vec3& a_scale);

	glm::mat4& getTransform() { return m_transform; }
	aie::ShaderProgram* getShader() const { return m_shader; }
	
protected:
	glm::mat4 m_transform;
	aie::OBJMesh* m_mesh; 
	aie::ShaderProgram* m_shader;
};
