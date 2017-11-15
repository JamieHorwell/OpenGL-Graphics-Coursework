#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "ResourceManager.h"
#include "../../nclgl/TextMesh.h"


class Renderer : public OGLRenderer {

public: 
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	


protected:
	ResourceManager resources;
	Camera* camera;


	//specific rendering
	void RenderSkyBox();
	void RenderWater();
	void RenderHeightMap();
	void RenderfboTest();
	void DrawTextOrth(const std::string &text, const Vector3 &position, const float size = 24.0f, const bool perspective = false);

	void createFBO(GLuint &FBOID);
	void createTexture(GLuint &TexID);
	void createDepthTexture(GLuint &TexID);
	void createDepthBufferAttachment(GLuint &depthBufferID);
	void bindFramebuffer(GLuint fboID);
	void bindScreenbuffer();

	void createWaterBuffers();


	GLuint cubeMap;
	Mesh* skyBox;
	HeightMap* waterMesh;
	HeightMap* terrain;
	Light* mainLight;


	Mesh* reflectionTest;
	Mesh* refractionTest;


	GLuint reflectionFBO;
	GLuint refractionFBO;

	GLuint reflectionTexture;
	GLuint refractionTexture;

	GLuint reflectionDepthTex;
	GLuint refractionDepthTex;


	Font* font;
	float time;
	int fps;
};