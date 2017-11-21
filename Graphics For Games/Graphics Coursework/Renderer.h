#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "ResourceManager.h"
#include "../../nclgl/TextMesh.h"
#include "WaterReflectRefract.h"
#include "Portal.h"
#include "ParticleEmitter.h"
#include "CameraTrail.h"


enum class SceneRender { Scene1, Scene2, Scene3 };

class Renderer : public OGLRenderer {

public: 
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	
	void RenderScene1();
	void RenderScene2();
	void RenderScene3();

	ResourceManager* getResources() { return &resources; };

	void createFBO(GLuint &FBOID);
	void createTexture(GLuint &TexID);
	void createDepthTexture(GLuint &TexID);
	void createDepthBufferAttachment(GLuint &depthBufferID);
	void bindFramebuffer(GLuint fboID);
	void bindScreenbuffer();

	Camera & getCam() { return *camera; };

protected:
	ResourceManager resources;
	Camera* camera;
	CameraTrail* camTrail;
	bool freeCam;


	//specific rendering
	void RenderSkyBox(GLuint skyboxTex);
	void RenderWater();
	void RenderHeightMap(HeightMap* heightMap);
	void RenderPortal();
	//Debugging relfection and refraction textures
	void RenderfboTest();
	


	void DrawTextOrth(const std::string &text, const Vector3 &position, const float size = 24.0f, const bool perspective = false);

	GLuint cubeMap;
	Mesh* skyBox;

	//Scene1 
	HeightMap* waterMesh;
	HeightMap* snowMountain;
	Light* mainLight;
	SceneNode* portalQuad;

	Mesh* reflectionTest;
	Mesh* refractionTest;
	

	WaterReflectRefract* reflectManager;


	//Scene2
	HeightMap* hellMountain;
	SceneNode* portalQuad2;
	ParticleEmitter* particleEmitter;

	//Scene2 RenderMethods
	void renderLava();

	Portal* portal;

	//Scene3



	//Scene independent
	Font* font;
	float time;
	int fps;
	SceneRender sceneOn;
	
};