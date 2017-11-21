#include "ParticleEmitter.h"
#include "Renderer.h"


ParticleEmitter::ParticleEmitter(Renderer* renderer, Shader* ParticleShader, GLuint particleTex, int maxParticles, int particleOutput, float gravity, float lifeTime, Vector3 emitLocation, Vector3 pSize)
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
	this->particleSize = pSize;
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

	//create temp viewMatrix to ignore rotation
	float camPitch = renderer->getCam().GetPitch();
	float camYaw = renderer->getCam().GetYaw();

	renderer->getCam().SetPitch(-camPitch);
	renderer->getCam().SetYaw(-camYaw);

	Matrix4 tempView = renderer->getCam().BuildViewMatrixNoRotation();

	for each(Particle * p in particles) {
		glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "modelViewMatrix"), 1, false, (float*)&particleModelView(*p));
		//glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "viewMatrix"), 1, false, (float*)&tempView);
		glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "diffuseTex"), 0);
		glUniform1f(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "lifetime"), p->lifeLength);
		particleMesh->Draw();
	}

	renderer->getCam().SetPitch(camPitch);
	renderer->getCam().SetYaw(camYaw);
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
		p->velocity = generateRandomDirection();
		p->pos = this->emitterStart;
		particles.push_back(p);
	}
}

Vector3 ParticleEmitter::generateRandomDirection()
{
	int XDir = rand() % 2;
	int ZDir = rand() % 2;
	int randomX = rand() % 400;
	int randomZ = rand() % 400;
	int randomY = rand() % 200 + 500;
	if (XDir < 1) {
		randomX = -randomX;
	}
	if (ZDir < 1) {
		randomZ = -randomZ;
	}
	return Vector3(randomX,randomY,randomZ);
}

Matrix4 ParticleEmitter::particleModelView(Particle & p)
{
	//set model matrix to position of particle
	Matrix4 modelMatrix = Matrix4::Translation(p.pos);
	Matrix4 viewMat = renderer->getViewMatrix();
	modelMatrix = modelMatrix * Matrix4::Scale(Vector3(15,15,15));
	Matrix4 modelView = viewMat * modelMatrix;
	modelView.values[0 + 0] = particleSize.x;
	modelView.values[0 + 1] = 0;
	modelView.values[0 + 2] = 0;
	modelView.values[(1 * 4) + 0] = 0;
	modelView.values[(1 * 4) + 1] = particleSize.y;
	modelView.values[(1 * 4) + 2] = 0;
	modelView.values[(2 * 4) + 0] = 0;
	modelView.values[(2 * 4) + 1] = 0;
	modelView.values[(2 * 4) + 2] = particleSize.z;
	//modelView * Matrix4::Scale(Vector3(15, 15, 15));
	return modelView;	
}
