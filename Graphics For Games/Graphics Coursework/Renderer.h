#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "ResourceManager.h"


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


	GLuint cubeMap;

	Mesh* skyBox;
	Mesh* waterMesh;


	HeightMap* terrain;

	Light* mainLight;
};