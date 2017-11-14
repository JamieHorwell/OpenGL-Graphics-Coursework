#pragma once
#include "../../nclgl/Shader.h"
#include "../../nclgl/Texture.h"


#include <unordered_map>
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	GLuint getTexture(string tex);

	Shader* getShader(string shader);

	bool addTexture(string texName);

	bool addShader(string shaderName, string vertexShader, string fragShader, string geomShader = "");

	bool addSkybox(string skyboxName,string negx, string posx, string posy, string negy, string negz, string posz);

	GLuint getSkybox(string skyboxName);

protected:

	std::unordered_map<string, Texture> textures;

	std::unordered_map<string, Shader*> shaders;

	std::unordered_map<string, GLuint> skyBoxes;

};

