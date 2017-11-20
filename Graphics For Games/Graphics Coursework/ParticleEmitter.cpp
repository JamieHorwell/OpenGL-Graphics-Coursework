#include "ParticleEmitter.h"
#include "Renderer.h"


ParticleEmitter::ParticleEmitter(Renderer* renderer, Shader* ParticleShader, GLuint particleTex, int maxParticles, int particleOutput, float gravity, float lifeTime, Vector3 emitLocation)
{
	this->renderer = renderer;
	this->particleShader = ParticleShader;
	this->particleTexture = particleTex;
	this->maxParticles = maxParticles;
	this->particleOutputRate = particleOutput;
	this->gravity = gravity;
	this->particleLifeTime = lifeTime;
	this->emitterStart = emitLocation;
	particleMesh = Mesh::GenerateQuad();
	particleMesh->SetTexture(particleTex);
	elapsedTime = 0;
}


ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::renderParticles()
{
	renderer->SetCurrentShader(this->particleShader);
	//set light here too?
	renderer->UpdateShaderMatrices();

	for each(Particle * p in particles) {
		glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "modelMatrix"), 1, false, (float*)&(Matrix4::Translation(p->pos) * Matrix4::Scale(Vector3(10,10,10))));
		glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "diffuseTex"), 0);
		glUniform1f(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "lifetime"), p->lifeLength);
		particleMesh->Draw();
	}
}

void ParticleEmitter::updateParticles(float msec)
{
	std::vector<Particle*>::iterator p;
	for (p = particles.begin(); p != particles.end();) {
		//particle has expired
		if ((*p)->lifeLength > this->particleLifeTime) {
			p = particles.erase(p);
		}
		else {
			//reduce velocity
			(*p)->velocity = (*p)->velocity * 0.996;
			//add y component due to gravity
			(*p)->velocity += Vector3(0, gravity * (msec / 1000), 0);
			//update position
			(*p)->pos += (*p)->velocity * (msec / 1000);
			//update lifetime
			(*p)->lifeLength += msec;
			++p;
		}
	}
	//add some more particles
	addParticles(msec);
}

void ParticleEmitter::addParticles(float msec)
{
	//elapsedTime += msec;
	int addPs = particleOutputRate / 1000 * msec;
	addPs++;
	for (int i = 0; i < addPs && particles.size() <= maxParticles; ++i) {
		Particle* p = new Particle;
		p->lifeLength = 0;
		p->velocity = Vector3(rand() % 200,500,rand() % 200);
		p->pos = this->emitterStart;
		particles.push_back(p);
	}
}
