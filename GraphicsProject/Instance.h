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
	Instance(glm::mat4 transform, aie::OBJMesh* mesh, aie::ShaderProgram* shader);
	Instance(const glm::vec3& position, const glm::vec3& eulerAngles, const glm::vec3& scale, aie::OBJMesh* mesh, aie::ShaderProgram* shader);
	~Instance();

	void draw(Scene* scene);

	// Create a transform with a set position, rotation, and scale
	static glm::mat4 createTransform(const glm::vec3& position, const glm::vec3& eulerAngles, const glm::vec3& scale);

	glm::mat4& getTransform() { return transform; }
	aie::ShaderProgram* getShader() const { return shader; }
	
protected:
	glm::mat4 transform;
	aie::OBJMesh* mesh; 
	aie::ShaderProgram* shader;
};
