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
			position(glm::vec3(0)), velocity(glm::vec3(0)), scale(1), color(glm::vec4(1)), life(0)
		{}

		glm::vec3 position;
		glm::vec3 velocity;

		float scale;

		glm::vec4 color;
		//decrements while alive until 0
		float life;
	};

	ParticleGenerator(const glm::vec3& position, Scene* scene, unsigned int maxParticles);
	~ParticleGenerator();

	// emisionRate is in particles/second
	void setup(float emisionRate, float lifeTime, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 acceleration, float startSpeed, float startScale, float endScale);

	void update(float deltaTime);
	void draw();

protected:
	unsigned int findUnusedParticle() const;
	

	glm::vec3 position;

	//is this generator emiting particles?
	bool isEmiting = false;
	std::vector<Particle> particles;
	//total number of particles in array
	unsigned int maxParticles;
	unsigned int aliveParticles;

	unsigned int particleVAO;
	unsigned int particleQuadBuffer;
	//used to stream particle position and scale to shader
	unsigned int particlePositionBuffer;
	float* particlePositionData;
	//used to stream particle color to shader
	unsigned int particleColorBuffer;
	float* particleColorData;

	aie::ShaderProgram* shader;
	Scene* scene;	//used to get camera


	float emisionRate;

	float lifeTime;
	glm::vec4 startColor;
	glm::vec4 endColor;
	glm::vec3 acceleration;
	float startSpeed;
	float startScale;
	float endScale;
};
