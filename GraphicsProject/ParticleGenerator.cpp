#include "ParticleGenerator.h"
#include "Shader.h"
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include "Camera.h"
#include "Scene.h"


ParticleGenerator::ParticleGenerator(const glm::vec3& position, Scene* scene, unsigned int particleCount) :
	emisionRate(0), lifeTime(0), startColor(glm::vec4(0)), endColor(glm::vec4(0)), acceleration(glm::vec3(0)), startSpeed(0), startScale(0), endScale(0)
{
	this->position = position;
	this->scene = scene;
	this->particleCount = particleCount;
	
	//fill vector with basic particles
	for (unsigned int i = 0; i < particleCount; i++)
	{
		particles.push_back(Particle());
	}


	//create particle mesh and atribute properties
	float particle_quad[] = {
		-0.5f,  0.5f, 0.f, 1.f,
		 0.5f,  0.5f, 0.f, 1.f,
		-0.5f, -0.5f, 0.f, 1.f,

		-0.5f,  -0.5f, 0.f, 1.f,
		 0.5f,   0.5f, 0.f, 1.f,
		 0.5f,  -0.5f, 0.f, 1.f
	};
	glGenBuffers(1, &particleVBO);
	glGenVertexArrays(1, &this->particleVAO);
	glBindVertexArray(this->particleVAO);
	//fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
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
	glDeleteVertexArrays(1, &particleVAO);
	glDeleteBuffers(1, &particleVBO);
	//particles vector holds values, so it just goes out of scope
}

void ParticleGenerator::setup(float emisionRate, float lifeTime, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 acceleration, float startSpeed, float startScale, float endScale)
{
	isEmiting = true;
	//convert from particles/second
	this->emisionRate = 1.f / emisionRate;

	this->lifeTime = lifeTime;

	this->startColor = startColor;
	this->endColor = endColor;

	this->acceleration = acceleration;
	this->startSpeed = startSpeed;

	this->startScale = startScale;
	this->endScale = endScale;
}


void ParticleGenerator::update(float deltaTime)
{
	if (!isEmiting)
	{
		return;
	}

	//time since the last particle was spawned
	static float timePassed = 0.0f;
	timePassed += deltaTime;

	//spawn particles after spawnDelay has passed
	while (timePassed > emisionRate)
	{
		timePassed -= emisionRate;

		//get index for an unused particle
		unsigned int index = findUnusedParticle();

		//temp particle values
		particles[index].life = lifeTime;
		particles[index].color = startColor;
		particles[index].position = position;

		//random xyz, then set magnitude to startSpeed
		glm::vec3 vel(rand() % 3 - 1, rand() % 3 - 1, rand() % 3 - 1);
		vel = glm::normalize(vel) * startSpeed;
		particles[index].velocity = vel;
	}

	//update particles
	for (auto& iter : particles)
	{
		//if particle is dead, skip it
		if (iter.life <= 0)
		{
			continue;
		}

		//update particle variables
		iter.life -= deltaTime;
		iter.position += iter.velocity * deltaTime;
		iter.velocity += acceleration * deltaTime;
		iter.color = glm::mix(endColor, startColor, iter.life / lifeTime);
		iter.scale = glm::mix(endScale, startScale, iter.life / lifeTime);
	}
}

void ParticleGenerator::draw()
{
	if (!isEmiting)
	{
		return;
	}

	shader->bind();
	//make particles stack alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);
	//since all particles use the same quad, bind it once
	glBindVertexArray(particleVAO);

	for (auto& iter : particles)
	{
		//skip dead particles
		if (iter.life <= 0)
		{
			continue;
		}

		//create billboard transform to face towards the camera
		glm::vec3 zAxis= glm::normalize(iter.position - scene->getCurrentCamera()->getPosition());
		glm::vec3 xAxis= glm::cross(glm::vec3(0, 1, 0), zAxis);
		glm::vec3 yAxis= glm::cross(zAxis, xAxis);
		glm::mat4 billboard(glm::vec4(xAxis, 0), glm::vec4(yAxis, 0), glm::vec4(zAxis, 0), glm::vec4(0,0,0,1));
		//translate and scale transform
		billboard = glm::translate(glm::mat4(1), iter.position) * billboard * glm::scale(glm::mat4(1), glm::vec3(iter.scale));

		//bind uniforms
		shader->bindUniform("ProjectionViewModel", scene->getCurrentCamera()->getProjectionMatrix(scene->getWindowSize().x, scene->getWindowSize().y) * scene->getCurrentCamera()->getViewMatrix() * billboard);
		shader->bindUniform("Color", iter.color);

		//draw quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//unbind quad
	glBindVertexArray(0);
	//undo shader effects
	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


unsigned int ParticleGenerator::findUnusedParticle() const
{
	static unsigned int lastUsedParticle = 0;

	//search from the last particle to the end of the vector
	for (unsigned int i = lastUsedParticle; i < particleCount; i++)
	{
		//dead particle, return it
		if (particles[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//no dead particles at the end, start again from the begining
	for (unsigned int i = 0; i < lastUsedParticle; i++)
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
