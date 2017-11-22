#include "shadowManager.h"
#include "Renderer.h"


shadowManager::shadowManager(Renderer* renderer)
{
	this->renderer = renderer;
	renderer->createFBO(shadowFBO, false, false, false);
	renderer->createDepthTexture(shadowTex);
}


shadowManager::~shadowManager()
{
}

void shadowManager::DrawShadowScene(SceneRender sceneToRender)
{
	renderer->bindFramebuffer(this->shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	//dont write any colours, just depth
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	renderer->SetCurrentShader(renderer->getResources()->getShader("shadowShader"));

	renderer->setViewMatrix(Matrix4::BuildViewMatrix(renderer->getMainLight()->GetPosition(), Vector3(0,0,0)));
	renderer->getTextMatRef() = biasMatrix*(renderer->getProjMatrix()*renderer->getViewMatrix());
	this->shadowTexMatrix = biasMatrix*(renderer->getProjMatrix()*renderer->getViewMatrix());
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
