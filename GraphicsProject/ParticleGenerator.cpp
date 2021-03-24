#include "ParticleGenerator.h"
#include "Shader.h"
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include "Camera.h"
#include "Scene.h"


ParticleGenerator::ParticleGenerator(const glm::vec3& position, Scene* scene, unsigned int maxParticles) :
	emisionRate(0), lifeTime(0), startColor(glm::vec4(0)), endColor(glm::vec4(0)), acceleration(glm::vec3(0)), startSpeed(0), startScale(0), endScale(0)
{
	this->position = position;
	this->scene = scene;
	this->maxParticles = maxParticles;
	this->aliveParticles = 0;

	this->particlePositionData = new float[maxParticles * 4];
	this->particleColorData = new float[maxParticles * 4];
	
	//fill vector with basic particles
	for (unsigned int i = 0; i < maxParticles; i++)
	{
		particles.push_back(Particle());
	}


	//bind VAO
	glGenVertexArrays(1, &this->particleVAO);
	glBindVertexArray(this->particleVAO);

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
	glGenBuffers(1, &particleQuadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleQuadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	//create pos buffer
	glGenBuffers(1, &particlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	//create color buffer
	glGenBuffers(1, &particleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	//set quad attribute on 0
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particleQuadBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//set pos attribute on 1
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//set color attribute on 2
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

	glVertexAttribDivisor(0, 0);	//use same quad for all particles
	glVertexAttribDivisor(1, 1);	//use each particle position
	glVertexAttribDivisor(2, 1);	//use each particle color

	//unbind VAO after done
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
	glDeleteBuffers(1, &particleQuadBuffer);
	glDeleteBuffers(1, &particlePositionBuffer);
	glDeleteBuffers(1, &particleColorBuffer);
	delete particlePositionData;
	delete particleColorData;

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

		//random point on a sphere radius startSpeed
		particles[index].velocity = glm::sphericalRand(startSpeed);
	}

	//reset count
	aliveParticles = 0;
	//update particles
	for (auto& iter : particles)
	{
		//if particle is dead, skip it
		if (iter.life <= 0)
		{
			continue;
		}

#pragma region UpdateBufferData
		//update data for buffers
		particlePositionData[aliveParticles * 4 + 0] = iter.position.x;
		particlePositionData[aliveParticles * 4 + 1] = iter.position.y;
		particlePositionData[aliveParticles * 4 + 2] = iter.position.z;
		particlePositionData[aliveParticles * 4 + 3] = iter.scale;
		particleColorData[aliveParticles * 4 + 0] = iter.color.r;
		particleColorData[aliveParticles * 4 + 1] = iter.color.g;
		particleColorData[aliveParticles * 4 + 2] = iter.color.b;
		particleColorData[aliveParticles * 4 + 3] = iter.color.a;
		//count how mnay particles are currently alive
		aliveParticles++;
#pragma endregion

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
	//bind VAO
	glBindVertexArray(particleVAO);


	//bind uniforms
	shader->bindUniform("CameraPosition", scene->getCurrentCamera()->getPosition());
	shader->bindUniform("ProjectionViewMatrix", scene->getCurrentCamera()->getProjectionMatrix(scene->getWindowSize().x, scene->getWindowSize().y) * scene->getCurrentCamera()->getViewMatrix());
	
	//set position buffer to position data
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //buffer orphaning
	glBufferSubData(GL_ARRAY_BUFFER, 0, aliveParticles * 4 * sizeof(GLfloat), particlePositionData);
	//set color buffer to color data
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //buffer orphaning
	glBufferSubData(GL_ARRAY_BUFFER, 0, aliveParticles * 4 * sizeof(GLfloat), particleColorData);

	//draw alive particles
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, aliveParticles);
	

	//unbind VAO
	glBindVertexArray(0);
	//undo shader effects
	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


unsigned int ParticleGenerator::findUnusedParticle() const
{
	static unsigned int lastUsedParticle = 0;

	//search from the last particle to the end of the vector
	for (unsigned int i = lastUsedParticle; i < maxParticles; i++)
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
