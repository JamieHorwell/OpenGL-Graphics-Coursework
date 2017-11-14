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

Shader* ResourceManager::getShader(string shader)
{
	return shaders.at(shader);
}

bool ResourceManager::addTexture(string texName)
{
	textures.emplace(texName,Texture(texName));
	if (textures.at(texName).getId()) return true;
	else return false;
}

bool ResourceManager::addShader(string shaderName, string vertexShader, string fragShader, string geomShader)
{
	shaders.emplace(shaderName, new Shader(SHADERDIR + vertexShader,SHADERDIR + fragShader));
	if (shaders.at(shaderName)->LinkProgram()) return true;
	else return false;
	
}

bool ResourceManager::addSkybox(string skyboxName, string negx, string posx, string posy, string negy, string negz, string posz)
{
	GLuint skyBox = SOIL_load_OGL_cubemap((TEXTUREDIR + negx).c_str(), (TEXTUREDIR + posx).c_str(), (TEXTUREDIR + posy).c_str(), (TEXTUREDIR + negy).c_str(), (TEXTUREDIR + negz).c_str(), (TEXTUREDIR + posz).c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	skyBoxes.emplace(skyboxName, skyBox);
	if (skyBoxes.at(skyboxName)) return true;
	else return false;
}

GLuint ResourceManager::getSkybox(string skyboxName)
{
	return skyBoxes.at(skyboxName);
}
