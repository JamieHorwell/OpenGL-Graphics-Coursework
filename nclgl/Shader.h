#pragma once
#include "OGLRenderer.h"

#define SHADER_VERTEX 0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2

using namespace std;

class Shader
{
public:
	Shader(string vertex, string fragment, string geometry = "");
	~Shader();

	GLuint GetProgram() { return program; };
	bool LinkProgram();

	string getName() { return name; };
	void setName(string name) { this->name = name; };

protected:
	void SetDefaultAttributes();
	bool LoadShaderFile(string from, string &into);
	GLuint GenerateShader(string from, GLenum type);

	GLuint objects[3];
	GLuint program;

	bool loadFailed;

	//to identify the shader
	string name;
};

