#include "PostProcessing.h"
#include "Renderer.h"


PostProcessing::PostProcessing(Renderer* renderer)
{
	postPasses = 0;
	screenQuad = Mesh::GenerateQuad();
	this->renderer = renderer;
	createPostProcessTex();
	//might be problem here
	createColorTex();
	//renderer->createTexture(bufferColourTex[0]);
	//renderer->createTexture(bufferColourTex[1]);
	
	//renderer->createFBO(bufferFBO);
	//renderer->createFBO(processFBO);


	//read scene into this
	glGenFramebuffers(1, &bufferFBO);
	//do post processing in this buffer
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0] || !bufferColourTex[1]) {
		return;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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
		case(SceneRender::Scene2) :
			renderer->setPostProcess(false);
			renderer->setPostProcessPass(true);
			renderer->RenderScene2(true);
			renderer->setPostProcess(true);
			renderer->setPostProcessPass(false);
			break;
		case(SceneRender::Scene3) :
			renderer->RenderScene3();
			break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
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
	Matrix4 tempProj = renderer->getProjMatrix();
	renderer->setProjMatrix(Matrix4::Orthographic(-1, 1, 1, -1, -1, 1));
	////make view matrix identity matrix as our quad will perfectly fill screen space
	renderer->getViewMatObj().ToIdentity();
	renderer->setModelMatrix(renderer->getViewMatrix());
	renderer->UpdateShaderMatrices();

	//give a temp proj matrix
	


	//will be drawing quad multiple times, so disable depth testing, make sure quad always drawn
	glDisable(GL_DEPTH_TEST);

	//get texel size, as tex coords from 0.0 to 1.0, so divide 1.0 by width and height
	glUniform2f(glGetUniformLocation(renderer->getCurrentShader()->GetProgram(), "pixelSize"), 1.0f / renderer->getWidth(), 1.0f / renderer->getHeight());

	 //2 pass gaussian blur, both passes performed by same shader, switched between axis by isVertical 
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
	renderer->setProjMatrix(tempProj);
}

void PostProcessing::DrawFinalScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	renderer->SetCurrentShader(renderer->getResources()->getShader("basicShader"));
	//make view matrix identity matrix as our quad will perfectly fill screen space
	Matrix4 tempViewMat = renderer->getProjMatrix();
	renderer->setProjMatrix(Matrix4::Orthographic(-1, 1, 1, -1, -1, 1));
	
	renderer->getViewMatObj().ToIdentity();
	renderer->setTextureMat(renderer->getViewMatrix());
	renderer->setModelMatrix(renderer->getViewMatrix());
	renderer->UpdateShaderMatrices();

	//give a temp proj matrix
	//glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "projMatrix"), 1, false, *&tempProjMatrix.values);

	//glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "viewMatrix"), 1, false, *&tempViewMat.values);
	screenQuad->SetTexture(bufferColourTex[0]);
	screenQuad->Draw();
	glUseProgram(0);
	renderer->setProjMatrix(tempViewMat);
	renderer->setViewMatrix(renderer->getCam().BuildViewMatrix());
}

void PostProcessing::createPostProcessTex()
{
	glGenTextures(1, &bufferDepthTex);

	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	//clamp tex coords
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//minified and magnified texture settings
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, renderer->getWidth(), renderer->getHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}

void PostProcessing::createColorTex()
{
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//minified and magnified texture settings
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderer->getWidth(), renderer->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	}
}
