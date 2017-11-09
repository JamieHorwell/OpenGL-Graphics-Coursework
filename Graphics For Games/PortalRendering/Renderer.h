#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include"../../nclgl/SceneNode.h"

class Renderer : public OGLRenderer {

public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void renderFromPortalViews();

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void moveLight();
	void DrawPortal();
	

	Matrix4 getPortalView(Matrix4 originalView, SceneNode* portalSrc, SceneNode* portalDest);

	void initPortal();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;

	HeightMap* heightMap;
	Mesh* quad;

	SceneNode* portalQuad;
	SceneNode* portalQuad2;

	Light* light;
	Camera* camera;
	
	Matrix4 cameraStart;


	GLuint cubeMap;

	float waterRotate;


};