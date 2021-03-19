#include "ParticleGenerator.h"
#include "Shader.h"
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include "Camera.h"
#include "Scene.h"


ParticleGenerator::ParticleGenerator(const glm::vec3& position, Scene* scene, float spawnRate, unsigned int particleCount)
{
	this->position = position;
	this->transform = createTransform(position, glm::vec3(0), glm::vec3(1));


	this->spawnDelay = 1 / spawnRate;	//convert from particles/second
	this->particleCount = particleCount;

	this->scene = scene;

	//fill vector with basic particles
	for (int i = 0; i < particleCount; i++)
	{
		particles.push_back(Particle());
	}


	//create particle mesh and atribute properties
	unsigned int VBO;
	float particle_quad[] = {
		-0.5f, 0.f, 0.5f,  1.f,
		0.5f,  0.f, 0.5f,  1.f,
		- 0.5f, 0.f, -0.5f, 1.f,

		- 0.5f, 0.f, -0.5f, 1.f,
		0.5f,  0.f, 0.5f,  1.f,
		0.5f,  0.f, -0.5f, 1.f
	};
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &this->particleVAO);
	glBindVertexArray(this->particleVAO);

	//fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	//set mesh atributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);



	//load shader
	shader = new aie::ShaderProgram();
	shader->loadShader(aie::eShaderStage::VERTEX, "./shaders/particle.vert");
	shader->loadShader(aie::eShaderStage::FRAGMENT, "./shaders/particle.frag");
	if (!shader->link())
	{
		printf("Particle shader has an error: %s\n", shader->getLastError());
	}
}

ParticleGenerator::~ParticleGenerator()
{
	delete shader;
	//particles vector holds values, so it just goes out of scope
}


void ParticleGenerator::update(float deltaTime)
{
	//time since the last particle was spawned
	static float timePassed = 0.0f;

	timePassed += deltaTime;

	//spawn particles after spawnDelay has passed
	while (timePassed > spawnDelay)
	{
		timePassed -= spawnDelay;

		//get index for an unused particle
		unsigned int index = findUnusedParticle();

		//temp particle values
		particles[index].life = 3.f;
		particles[index].color = glm::vec4(1, 0, 0, 1);
		particles[index].position = position;
		particles[index].velocity = glm::vec3(0, -1, 0);
	}

	//update particles
	for (auto& iter : particles)
	{
		//if particle is dead, skip it
		if (iter.life <= 0)
		{
			continue;
		}

		//update life and position
		iter.life -= deltaTime;
		iter.position += iter.velocity * deltaTime;


		//	--- update custom values ---

		iter.color = glm::vec4(sinf(iter.life / 3.f), 0, 0, 1);
	}
}

void ParticleGenerator::draw()
{
	shader->bind();

	//causes particles to stack, making them 'glow'
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);


	for (auto iter : particles)
	{
		//skip dead particles
		if (iter.life <= 0)
		{
			continue;
		}

		shader->bindUniform("ProjectionViewModel", scene->getCurrentCamera()->getProjectionMatrix(scene->getWindowSize().x, scene->getWindowSize().y) * scene->getCurrentCamera()->getViewMatrix() * createTransform(iter.position, glm::vec3(0), glm::vec3(1)));
		shader->bindUniform("Color", iter.color);

		glBindVertexArray(particleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	//undo 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


unsigned int ParticleGenerator::findUnusedParticle()
{
	static unsigned int lastUsedParticle = 0;

	//search from the last particle to the end of the vector
	for (int i = lastUsedParticle; i < particleCount; i++)
	{
		//dead particle, return it
		if (particles[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//no dead particles at the end, start again from the begining
	for (int i = 0; i < lastUsedParticle; i++)
	{
		//dead particle, return it
		if (particles[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//no dead particles. this means too many particles are being spawned
	//reuse the first particle in this case
	lastUsedParticle = 0;
	return 0;
}

glm::mat4 ParticleGenerator::createTransform(const glm::vec3& position, const glm::vec3& eulerAngles, const glm::vec3& scale)
{
	//use glm functions to create transform matrix
	return glm::translate(glm::mat4(1), position)
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.z), glm::vec3(0, 0, 1))
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.y), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.x), glm::vec3(1, 0, 0))
		* glm::scale(glm::mat4(1), scale);
}
