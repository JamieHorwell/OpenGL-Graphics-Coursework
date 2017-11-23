#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer;

enum class SceneRender;

class PostProcessing
{
public:
	PostProcessing(Renderer * renderer);
	~PostProcessing();

	void DrawScene(SceneRender scene);
	void DrawPostProcess();
	void DrawFinalScreen();

	
	GLuint getProcessFBO() { return processFBO; };
	GLuint getbufferFBO() { return bufferFBO; };


	void createPostProcessTex();
	void createColorTex();

	
	void setPostProcess(int passes) { this->postPasses = passes; };

private:
	Renderer* renderer;

	Mesh* screenQuad;
	
	int postPasses;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;


};

