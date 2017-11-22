#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer;

enum class SceneRender;

class shadowManager
{
public:
	shadowManager(Renderer* renderer);
	~shadowManager();

	void DrawShadowScene(SceneRender sceneToRender);

	GLuint getShadowTex() { return shadowTex; };

	Matrix4 & getShadowTexMatrix() { return shadowTexMatrix; };


protected:
	GLuint shadowTex;
	GLuint shadowFBO;

	Renderer* renderer;

	Matrix4 shadowTexMatrix;
};

