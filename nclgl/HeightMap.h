#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "Mesh.h"

#define RAW_WIDTH 513
#define RAW_HEIGHT 513

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 8.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f


class HeightMap : public Mesh
{
public:
	//initialise heightmap with data
	HeightMap(std::string name);
	HeightMap();
	~HeightMap();

	GLuint GetTopTex() { return topTex; };
	void SetTopTex(GLuint topTex) { this->topTex = topTex; };

	GLuint GetTopTexBump() { return topTexBump; };
	void SetTopTexBump(GLuint topTexBump) { this->topTexBump = topTex; };

protected:
	GLuint topTex;
	GLuint topTexBump;
};

