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

	GLuint getShader(string shader);

	bool addTexture(string texName);

	bool addShader(string shaderName, string vertexShader, string fragShader, string geomShader = "");



protected:

	std::unordered_map<string, Texture> textures;

	std::unordered_map<string, Shader*> shaders;
};

