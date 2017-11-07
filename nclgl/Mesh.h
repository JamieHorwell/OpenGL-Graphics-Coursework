#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	void SetTexture(GLuint tex) { texture = tex; };
	GLuint GetTexture() { return texture; };

protected:
	void BufferData();
	//rebuffer data
	void GenerateNormals();


	//VAO stores state needed to supply vertex data, stores format of vertex data as well as buffer objects providing the vertex data arrays
	//bing it to VAO to then manipulate it
	GLuint arrayObject;
	//buffer objects store an array of unformatted memory allocated by OpenGL context, used to store the vertex data, we store all the properties up to MAX_BUFFER
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint numIndices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;
	unsigned int* indices;

	//texture info
	GLuint texture;
	Vector2* textureCoords;
	Vector3* normals;
};

