/*  Created: 19/3/2021
 *  Author: Thomas Dufresne
 *
 *  Last Modified: 8/4/2021
 *
 *	GPU instanced particle generator with billboarding. After instantiating the class, 
 *	setup() is used to start emitting particles, and sets the particle variables.
 */
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
			m_position(glm::vec3(0)), m_velocity(glm::vec3(0)), m_life(0)
		{}

		glm::vec3 m_position;
		glm::vec3 m_velocity;
		//decrements while alive until 0
		float m_life;

		//particles dont need to store color or scale as they are determined by its lifetime
	};

	ParticleGenerator(const glm::vec3& a_emitterPosition, Scene* a_scene, unsigned int a_maxParticles);
	~ParticleGenerator();

	// Setup generator to start emitting particles
	// EmisionRate is in particles/second
	void setup(float a_emisionRate, float a_lifeTime, glm::vec4 a_startColor, glm::vec4 a_endColor, glm::vec3 a_acceleration, float a_startSpeed, float a_startScale, float a_endScale);

	void update(float a_deltaTime);
	void draw();

protected:
	// Find the index of a dead particle
	// If no dead particles exist, returns -1
	int findUnusedParticle() const;
	

	glm::vec3 m_emitterPosition;

	//is this generator emiting particles?
	bool m_isEmiting = false;
	std::vector<Particle> m_particles;
	//total number of particles in array
	unsigned int m_maxParticles;
	unsigned int m_aliveParticles;

	unsigned int m_particleVAO;
	unsigned int m_particleQuadBuffer;
	//used to stream particle position and scale to shader
	unsigned int m_particlePositionBuffer;
	float* m_particlePositionData;
	//used to stream particle color to shader
	unsigned int m_particleColorBuffer;
	float* m_particleColorData;

	aie::ShaderProgram* m_shader;
	Scene* m_scene;	//used to get camera


	float m_emisionRate;

	float m_lifeTime;
	glm::vec4 m_startColor;
	glm::vec4 m_endColor;
	glm::vec3 m_acceleration;
	float m_startSpeed;
	float m_startScale;
	float m_endScale;
};
