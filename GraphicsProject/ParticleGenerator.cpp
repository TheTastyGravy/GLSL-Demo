#include "ParticleGenerator.h"
#include "Shader.h"
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include "Camera.h"
#include "Scene.h"


ParticleGenerator::ParticleGenerator(const glm::vec3& a_emitterPosition, Scene* a_scene, unsigned int a_maxParticles) :
	m_emisionRate(0), m_lifeTime(0), m_startColor(glm::vec4(0)), m_endColor(glm::vec4(0)), m_acceleration(glm::vec3(0)), m_startSpeed(0), m_startScale(0), m_endScale(0)
{
	m_emitterPosition = a_emitterPosition;
	m_scene = a_scene;
	m_maxParticles = a_maxParticles;
	m_aliveParticles = 0;

	m_particlePositionData = new float[m_maxParticles * 4];
	m_particleColorData = new float[m_maxParticles * 4];
	
	//fill vector with basic particles
	for (unsigned int i = 0; i < m_maxParticles; i++)
	{
		m_particles.push_back(Particle());
	}


	//bind VAO
	glGenVertexArrays(1, &this->m_particleVAO);
	glBindVertexArray(this->m_particleVAO);

	//create particle mesh and atribute properties
	float particle_quad[] = 
	{
		-0.5f,  0.5f, 0.f, 1.f,
		 0.5f,  0.5f, 0.f, 1.f,
		-0.5f, -0.5f, 0.f, 1.f,

		-0.5f,  -0.5f, 0.f, 1.f,
		 0.5f,   0.5f, 0.f, 1.f,
		 0.5f,  -0.5f, 0.f, 1.f
	};
	//create and fill quad buffer
	glGenBuffers(1, &m_particleQuadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleQuadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	//create pos buffer
	glGenBuffers(1, &m_particlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	//create color buffer
	glGenBuffers(1, &m_particleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	//set quad attribute on 0
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleQuadBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//set pos attribute on 1
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_particlePositionBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//set color attribute on 2
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleColorBuffer);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

	glVertexAttribDivisor(0, 0);	//use same quad for all particles
	glVertexAttribDivisor(1, 1);	//use each particle position
	glVertexAttribDivisor(2, 1);	//use each particle color

	//unbind VAO after done
	glBindVertexArray(0);


	//load shader
	m_shader = new aie::ShaderProgram();
	m_shader->loadShader(aie::eShaderStage::VERTEX, "./shaders/particle.vert");
	m_shader->loadShader(aie::eShaderStage::FRAGMENT, "./shaders/particle.frag");
	if (!m_shader->link())
	{
		printf("Particle shader has an error: %s\n", m_shader->getLastError());
	}
}

ParticleGenerator::~ParticleGenerator()
{
	delete m_shader;
	glDeleteVertexArrays(1, &m_particleVAO);
	glDeleteBuffers(1, &m_particleQuadBuffer);
	glDeleteBuffers(1, &m_particlePositionBuffer);
	glDeleteBuffers(1, &m_particleColorBuffer);
	delete m_particlePositionData;
	delete m_particleColorData;

	//particles vector holds values, so it just goes out of scope
}

void ParticleGenerator::setup(float a_emisionRate, float a_lifeTime, glm::vec4 a_startColor, glm::vec4 a_endColor, glm::vec3 a_acceleration, float a_startSpeed, float a_startScale, float a_endScale)
{
	m_isEmiting = true;
	//convert from particles/second
	m_emisionRate = 1.f / a_emisionRate;

	m_lifeTime = a_lifeTime;

	m_startColor = a_startColor;
	m_endColor = a_endColor;

	m_acceleration = a_acceleration;
	m_startSpeed = a_startSpeed;

	m_startScale = a_startScale;
	m_endScale = a_endScale;
}


void ParticleGenerator::update(float a_deltaTime)
{
	if (!m_isEmiting)
	{
		return;
	}

	//time since the last particle was spawned
	static float timePassed = 0.0f;
	timePassed += a_deltaTime;

	//spawn particles after spawnDelay has passed
	while (timePassed > m_emisionRate)
	{
		timePassed -= m_emisionRate;

		//get index for a dead particle
		int index = findUnusedParticle();
		//if there are no dead particles, dont create any new ones
		if (index == -1)
		{
			break;
		}

		//set starting particle values
		m_particles[index].m_life = m_lifeTime;
		m_particles[index].m_position = m_emitterPosition;

		//random point on a sphere radius startSpeed
		m_particles[index].m_velocity = glm::sphericalRand(m_startSpeed);
	}

	//reset count
	m_aliveParticles = 0;
	//update particles
	for (auto& particle : m_particles)
	{
		//if particle is dead, skip it
		if (particle.m_life <= 0)
		{
			continue;
		}


		//update particle variables
		particle.m_life -= a_deltaTime;
		particle.m_velocity += m_acceleration * a_deltaTime;
		particle.m_position += particle.m_velocity * a_deltaTime;

		//the color is only determined by a particles lifetime
		glm::vec4 particleColor = glm::mix(m_endColor, m_startColor, particle.m_life / m_lifeTime);

#pragma region UpdateBufferData
		//update data for buffers
		m_particlePositionData[m_aliveParticles * 4 + 0] = particle.m_position.x;
		m_particlePositionData[m_aliveParticles * 4 + 1] = particle.m_position.y;
		m_particlePositionData[m_aliveParticles * 4 + 2] = particle.m_position.z;
		//The last value of the position buffer is the scale
		m_particlePositionData[m_aliveParticles * 4 + 3] = glm::mix(m_endScale, m_startScale, particle.m_life / m_lifeTime);
		m_particleColorData[m_aliveParticles * 4 + 0] = particleColor.r;
		m_particleColorData[m_aliveParticles * 4 + 1] = particleColor.g;
		m_particleColorData[m_aliveParticles * 4 + 2] = particleColor.b;
		m_particleColorData[m_aliveParticles * 4 + 3] = particleColor.a;
#pragma endregion

		//count how mnay particles are currently alive
		m_aliveParticles++;
	}
}

void ParticleGenerator::draw()
{
	if (!m_isEmiting)
	{
		return;
	}

	m_shader->bind();
	//make particles stack with alpha blending
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//bind VAO
	glBindVertexArray(m_particleVAO);


	//bind uniforms
	m_shader->bindUniform("CameraPosition", m_scene->getCurrentCamera()->getPosition());
	m_shader->bindUniform("ProjectionViewMatrix", m_scene->getCurrentCamera()->getProjectionMatrix(m_scene->getWindowSize()) * m_scene->getCurrentCamera()->getViewMatrix());
	
	//set position buffer to position data
	glBindBuffer(GL_ARRAY_BUFFER, m_particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //buffer orphaning
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_aliveParticles * 4 * sizeof(GLfloat), m_particlePositionData);
	//set color buffer to color data
	glBindBuffer(GL_ARRAY_BUFFER, m_particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //buffer orphaning
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_aliveParticles * 4 * sizeof(GLfloat), m_particleColorData);

	//draw alive particles
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_aliveParticles);
	

	//unbind VAO
	glBindVertexArray(0);
	//reset defaults
	glDepthMask(true);
	glBlendFunc(GL_ONE, GL_ZERO);
}


int ParticleGenerator::findUnusedParticle() const
{
	static unsigned int lastUsedParticle = 0;

	//search from the last particle to the end of the vector
	for (unsigned int i = lastUsedParticle; i < m_maxParticles; i++)
	{
		//dead particle, return it
		if (m_particles[i].m_life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//no dead particles at the end, start again from the begining
	for (unsigned int i = 0; i < lastUsedParticle; i++)
	{
		//dead particle, return it
		if (m_particles[i].m_life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//no dead particles. this means too many particles are being spawned
	//reset starting index
	lastUsedParticle = 0;
	return -1;
}
