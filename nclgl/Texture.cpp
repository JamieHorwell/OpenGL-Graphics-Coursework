#include "Texture.h"

const string Texture::TexDir = "../../Textures/";

Texture::Texture(string texName)
{
	this->texName = texName;
	this->textureId = SOIL_load_OGL_texture((TEXTUREDIR + texName).c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
}




Texture::~Texture()
{
}
