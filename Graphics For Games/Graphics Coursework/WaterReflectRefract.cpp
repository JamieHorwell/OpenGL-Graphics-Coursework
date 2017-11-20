#include "WaterReflectRefract.h"
#include "Renderer.h"


WaterReflectRefract::WaterReflectRefract(Renderer* renderer)
{
	this->renderer = renderer;
}


WaterReflectRefract::~WaterReflectRefract()
{
}

void WaterReflectRefract::setPlaneToClip(float height, bool reflect)
{
	if (reflect) {
		this->planeToClip = Vector4(0, 1, 0, -height);
	}
	else {
		this->planeToClip = Vector4(0, -1, 0, height);
	}
}


void WaterReflectRefract::cameraReflectionPos(Camera& camera, Matrix4& viewMat)
{
	savedCamDist  = 2 * (camera.GetPosition().y - 150);
	camera.invertPitch();
	camera.SetPosition(camera.GetPosition() - Vector3(0, savedCamDist, 0));

	viewMat = camera.BuildViewMatrix();
}

void WaterReflectRefract::resetCamera(Camera& camera, Matrix4 & viewMat)
{
	camera.invertPitch();
	camera.SetPosition(camera.GetPosition() + Vector3(0, savedCamDist, 0));

	viewMat = camera.BuildViewMatrix();
}


void WaterReflectRefract::createWaterBuffers()
{
	renderer->createFBO(reflectionFBO);
	renderer->bindFramebuffer(reflectionFBO);
	renderer->createTexture(reflectionTexture);
	renderer->createDepthTexture(reflectionDepthTex);
	renderer->bindScreenbuffer();


	renderer->createFBO(refractionFBO);
	renderer->bindFramebuffer(refractionFBO);
	renderer->createTexture(refractionTexture);
	renderer->createDepthBufferAttachment(refractionDepthTex);
	renderer->bindScreenbuffer();
}


