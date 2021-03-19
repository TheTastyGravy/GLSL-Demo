#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace aie
{
	class ShaderProgram;
}
class Scene;

class ParticleGenerator
{
public:
	struct Particle
	{
		Particle() :
			position(glm::vec3(0)), velocity(glm::vec3(0)), color(glm::vec4(1)), life(0)
		{}

		glm::vec3 position;
		glm::vec3 velocity;

		glm::vec4 color;
		//decrements while alive until 0
		float life;
	};

	// spawnRate is particles/second
	ParticleGenerator(const glm::vec3& position, Scene* csceneam, float spawnRate, unsigned int particleCount);
	~ParticleGenerator();

	void update(float deltaTime);
	void draw();

protected:
	unsigned int findUnusedParticle();

	glm::mat4 createTransform(const glm::vec3& position, const glm::vec3& eulerAngles, const glm::vec3& scale);
	

	float spawnDelay;

	glm::vec3 position;
	glm::mat4 transform;

	std::vector<Particle> particles;
	unsigned int particleCount;

	unsigned int particleVAO;

	aie::ShaderProgram* shader;

	Scene* scene;

};
