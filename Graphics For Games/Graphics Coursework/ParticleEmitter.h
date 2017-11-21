#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer;

struct Particle {
	Vector3 pos;
	Vector3 velocity;
	float lifeLength;
};


class ParticleEmitter
{
public:
	ParticleEmitter(Renderer* renderer, Shader* particleShader, GLuint particleTex, int maxParticles, int particleOutput, float gravity, float lifeTime, Vector3 emitLocation, Vector3 pSize);
	~ParticleEmitter();

	//loop through all particles, render them
	void renderParticles();

	//update all particles lifeLength, velocities
	void updateParticles(float msec);

	void addParticles(float msec);


	void setRenderer(Renderer* renderer) { this->renderer = renderer; };

	Vector3 generateRandomDirection();

	Matrix4 particleModelView(Particle & p);


protected: 
	Mesh* particleMesh;

	std::vector<Particle*> particles;

	float particleLifeTime;
	float gravity;
	float elapsedTime;
	int particleOutputRate;
	int maxParticles;
	Vector3 emitterStart;
	Vector3 particleSize;

	Renderer* renderer;

	//
	Shader* particleShader;
	GLuint particleTexture;
};

