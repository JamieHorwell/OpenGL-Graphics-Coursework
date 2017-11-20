#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"


class Renderer;

class WaterReflectRefract
{
public:
	WaterReflectRefract(Renderer* renderer);
	~WaterReflectRefract();

	
	void setPlaneToClip(float height, bool reflect);
	Vector4 getPlaneToClip() { return planeToClip; };


	 void cameraReflectionPos(Camera& camera, Matrix4& viewMat);
	 void resetCamera(Camera& camera, Matrix4& viewMat);

	 void createWaterBuffers();
	

	 GLuint getReflectionFBO() { return reflectionFBO; };
	 GLuint getRefractionFBO() { return refractionFBO; };

	 GLuint getReflectionTexture() { return reflectionTexture; };
	 GLuint getRefractionTexture() { return refractionTexture; };

	 GLuint getReflectionDepthTex() { return reflectionDepthTex; };
	 GLuint getRefractionDepthTex() { return refractionDepthTex; };

protected:
	Vector4 planeToClip;
	
	float savedCamDist;

	GLuint reflectionFBO;
	GLuint refractionFBO;

	GLuint reflectionTexture;
	GLuint refractionTexture;

	GLuint reflectionDepthTex;
	GLuint refractionDepthTex;

	Renderer* renderer;
};

