#include "PostProcessing.h"
#include "Renderer.h"


PostProcessing::PostProcessing(Renderer* renderer)
{
	postPasses = 0;
	screenQuad = Mesh::GenerateQuad();
	this->renderer = renderer;
	renderer->createDepthTexture(bufferDepthTex);
	renderer->createTexture(bufferColourTex[0]);
	renderer->createTexture(bufferColourTex[1]);
	
	renderer->createFBO(bufferFBO);
	renderer->createFBO(processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
}


PostProcessing::~PostProcessing()
{
}

void PostProcessing::DrawScene(SceneRender scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	switch (scene) {

		case(SceneRender::Scene1):
			renderer->RenderScene1(true);
			break;
		case(SceneRender::Scene2):
			renderer->RenderScene2(true,true);
			break;
	}

	
}

void PostProcessing::DrawPostProcess()
{
	//bind out processFBO to draw into
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	//attach bufferColourTex[1] to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	//clear FBO
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	renderer->SetCurrentShader(renderer->getResources()->getShader("processShader"));
	//orthographic matrix from -1.0 to 1.0 on each axis
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	//make view matrix identity matrix as our quad will perfectly fill screen space
	viewMatrix.ToIdentity();
	renderer->UpdateShaderMatrices();

	//will be drawing quad multiple times, so disable depth testing, make sure quad always drawn
	glDisable(GL_DEPTH_TEST);

	//get texel size, as tex coords from 0.0 to 1.0, so divide 1.0 by width and height
	glUniform2f(glGetUniformLocation(renderer->getCurrentShader()->GetProgram(), "pixelSize"), 1.0f / renderer->getWidth(), 1.0f / renderer->getHeight());

	// 2 pass gaussian blur, both passes performed by same shader, switched between axis by isVertical 
	for (int i = 0; i < postPasses; ++i) {
		//bind bufferColourTex[1] as colour attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);


		glUniform1i(glGetUniformLocation(renderer->getCurrentShader()->GetProgram(), "isVertical"), 0);
		screenQuad->SetTexture(bufferColourTex[0]);
		screenQuad->Draw();
		//swap colour buffers and do second blur pass on opposite axis
		glUniform1i(glGetUniformLocation(renderer->getCurrentShader()->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		screenQuad->SetTexture(bufferColourTex[1]);
		screenQuad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void PostProcessing::DrawFinalScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	renderer->SetCurrentShader(renderer->getResources()->getShader("sceneShader"));
	renderer->UpdateShaderMatrices();
	//set proj matrix
	//set view matrix
	screenQuad->SetTexture(bufferColourTex[0]);
	screenQuad->Draw();
	glUseProgram(0);
}
