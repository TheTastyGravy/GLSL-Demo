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
			position(glm::vec3(0)), velocity(glm::vec3(0)), life(0)
		{}

		glm::vec3 position;
		glm::vec3 velocity;
		//decrements while alive until 0
		float life;

		//particles dont need to store color or scale as they are determined by its lifetime
	};

	ParticleGenerator(const glm::vec3& emitterPosition, Scene* scene, unsigned int maxParticles);
	~ParticleGenerator();

	// Setup generator to start emitting particles
	// EmisionRate is in particles/second
	void setup(float emisionRate, float lifeTime, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 acceleration, float startSpeed, float startScale, float endScale);

	void update(float deltaTime);
	void draw();

protected:
	// Find the index of a dead particle
	// If no dead particles exist, returns -1
	int findUnusedParticle() const;
	

	glm::vec3 emitterPosition;

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
