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


	int portal_intersection(Vector3 pos1, Vector3 pos2, SceneNode* portal);


	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* mountainShader;

	HeightMap* heightMap;
	Mesh* quad;

	SceneNode* portalQuad;
	SceneNode* portalQuad2;

	Light* light;
	Camera* camera;

	Matrix4 cameraStart;

	GLuint toptex;
	GLuint cubeMap;

	float waterRotate;


};