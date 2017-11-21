#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/SceneNode.h"

class Renderer;

enum class SceneRender;

class Portal
{
public:
	Portal(Renderer* renderer);
	~Portal();

	Matrix4 getPortalView(Matrix4 originalView, SceneNode* portalSrc, SceneNode* portalDest);

	int portal_intersection(Vector3 pos1, Vector3 pos2, SceneNode* portal);

	void renderFromPortalView(SceneNode* portalSrc, SceneNode* portalDest, bool reflection, SceneRender sceneToRender);

	void teleport(Matrix4 prevCam, Matrix4 currentCam);


protected:
	

	SceneNode* portal1;

	SceneNode* portal2;

	Renderer* renderer;
};

