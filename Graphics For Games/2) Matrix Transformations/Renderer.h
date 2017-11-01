#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"


class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();

	void moveTexture();


protected:
	Mesh* triangle;

	bool filtering;
	bool repeating;

};