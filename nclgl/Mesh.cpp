#include "Mesh.h"



Mesh::Mesh()
{
	for (int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}
	//generate name for our VAO, by passing GLuint by reference, this is how OpenGL handles name generation of 'Objects'
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	texture = 0;
	textureCoords = NULL;
	type = GL_TRIANGLES;
}


Mesh::~Mesh()
{
	//delete VAO and VBOS
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteTextures(1, &texture);
	delete[]vertices;
	delete[]colours;
	delete[]textureCoords;

}

void Mesh::Draw()
{
	//bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);
	//bind our VAO again
	glBindVertexArray(arrayObject);
	//type of primitive to draw, first vertex to draw from, how many vertices to draw
	glDrawArrays(type, 0, numVertices);
	//unbind VAO
	glBindVertexArray(0);
	//unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh * Mesh::GenerateTriangle()
{
	Mesh* m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh * Mesh::GenerateQuad()
{
	Mesh* m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	m->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	m->BufferData();
	return m;
}

void Mesh::BufferData()
{
	//bind this meshes array object, now any vertex array functionality will be performed on the newly bund array object
	glBindVertexArray(arrayObject);

	//generate new VBO, store its name in first index of our bufferObject array
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	//bind it, so all vertex buffer functions will be applied to our new VBO,  and also assigns new VBO to to currently bound VAO, which will tie all VBOs into our VAO
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	//now buffer the vertex position data, which copies data into graphics memory, second param tells how much memory we need in graphics memory to store this
	//and the pointer to the data to pass, finally inform OpenGL how we expect data to be used, either dynamically updated, or loaded once as static data
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	//now VBO data copied into memory, set how to access it, by modifying VAO.
	//tell OpenGL vertex attrib has 3 float components per vertex
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//next lines enables this
	glEnableVertexAttribArray(VERTEX_BUFFER);

	//now do same for textureCoords
	if (textureCoords) {
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}


	//now do the same for colours
	if (colours) {
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	glBindVertexArray(0);
}
