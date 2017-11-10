#include "Texture.h"

const string Texture::TexDir = "../../Textures/";

Texture::Texture(string texName)
{
	this->texName = texName;
	const char *texFile = (TexDir + texName).c_str();
	this->textureId = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
}




Texture::~Texture()
{
}
