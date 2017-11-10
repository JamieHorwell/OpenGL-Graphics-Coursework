#pragma once
#include "OGLRenderer.h"
class Texture
{
public:
	Texture(string texName);
	~Texture();

	GLuint getId() { return textureId; };

	string getName() { return texName; };



private:
	GLuint textureId;

	string texName;


	static const string TexDir;
};

