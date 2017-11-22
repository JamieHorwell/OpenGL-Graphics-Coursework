#include "shadowManager.h"
#include "Renderer.h"


shadowManager::shadowManager(Renderer* renderer)
{
	this->renderer = renderer;
	renderer->createFBO(shadowFBO, false, false, false);
	createShadowTexture();
}


shadowManager::~shadowManager()
{
}

void shadowManager::DrawShadowScene(SceneRender sceneToRender)
{
	renderer->bindFramebuffer(this->shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, shadowSize, shadowSize);

	//dont write any colours, just depth
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	//renderer->SetCurrentShader(renderer->getResources()->getShader("shadowShader"));

	
	this->lightViewMatrix = Matrix4::BuildViewMatrix(renderer->getMainLight()->GetPosition(), Vector3(4000, 0, 4000));



	renderer->getTextMatRef() = biasMatrix*(renderer->getProjMatrix()*this->lightViewMatrix);
	this->shadowTexMatrix = biasMatrix*(renderer->getProjMatrix()*lightViewMatrix);
	renderer->UpdateShaderMatrices();

	switch(sceneToRender) {
		case(SceneRender::Scene1) :
			renderer->RenderScene1(false, true);
			break;
		case(SceneRender::Scene2) :
			renderer->RenderScene2(false, true);
			break;
	}

	glUseProgram(0);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glViewport(0,0,renderer->getWidth(),renderer->getHeight());

	renderer->setViewMatrix(renderer->getCam().BuildViewMatrix());

	renderer->bindScreenbuffer();
}

void shadowManager::createShadowTexture()
{
	//create shadow tex
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	//bind to shadow framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
