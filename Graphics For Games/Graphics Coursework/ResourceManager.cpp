#include "ResourceManager.h"



ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

GLuint ResourceManager::getTexture(string tex)
{
	return textures.at(tex).getId();
}

GLuint ResourceManager::getShader(string shader)
{
	return shaders.at("shader")->GetProgram();
}

bool ResourceManager::addTexture(string texName)
{
	textures.emplace(texName,Texture(texName));
	if (textures.at(texName).getId()) return true;
	else return false;
}

bool ResourceManager::addShader(string shaderName, string vertexShader, string fragShader, string geomShader)
{
	shaders.emplace(shaderName, new Shader(SHADERDIR + vertexShader,SHADERDIR + fragShader,SHADERDIR + geomShader));
	if (shaders.at(shaderName)->LinkProgram()) return true;
	else return false;
	
}
