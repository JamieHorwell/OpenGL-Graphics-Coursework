#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer;


class shadowManager
{
public:
	shadowManager();
	~shadowManager();



protected:
	GLuint shadowTex;
	GLuint shadowFBO;
};

