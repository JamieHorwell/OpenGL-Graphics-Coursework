#include "Renderer.h"
#include "../../nclgl/HeightMap.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera(-8.0f,40.0f,Vector3(-200.0f,50.0f,250.0f));
	light = new Light(Vector3(1000.0f,3000.0f,1000.0f), Vector4(1,1,1,1), 1000500.0f);
	/*light = new Light(Vector3(-450.0f, 200.0f, 280.0f),
		Vector4(1, 1, 1, 1), 5500.0f);*/
	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",SHADERDIR"shadowscenefrag.glsl");

	shadowShader = new Shader(SHADERDIR"shadowVert.glsl",SHADERDIR"shadowFrag.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram()) {
		return;
	}

	//create depth texture
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);



	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	floor = new HeightMap(TEXTUREDIR"snowMountain.raw");
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(floor->GetTexture(), true);
	SetTextureRepeating(floor->GetBumpMap(), true);
	glEnable(GL_DEPTH_TEST);

	projMatrix = Matrix4::Perspective(1.0f,15000.0f,(float)width / (float)height,45.0f);

	init = true;

	

}

Renderer::~Renderer(void) {
	glDeleteTextures(1,&shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete camera;
	delete light;
	delete hellData;
	delete hellNode;
	delete floor;

	delete sceneShader;
	delete shadowShader;
	currentShader = NULL;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	hellNode->Update(msec);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		light->SetPosition(light->GetPosition() + Vector3(5,0,0));
	}
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		light->SetPosition(light->GetPosition() - Vector3(5, 0, 0));
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawShadowScene();
	DrawCombinedScene();

	SwapBuffers();
}

void Renderer::DrawShadowScene() {
	//need an FBO to fill what our light sees
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	//temporarily increase window size to accomdate texture size of the for the shadowmap
	glViewport(0,0,SHADOWSIZE, SHADOWSIZE);

	//we dont want to write any colors, just depth
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


	SetCurrentShader(shadowShader);

	//build viewMatrix from lights position, set it to look at origin, where hellknight is standing
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(4000,0,4000));
	//bias matrix keeps coords to 0.0 to 1.0 range for suitable texture sampling
	textureMatrix = biasMatrix*(projMatrix*viewMatrix);

	UpdateShaderMatrices();

	DrawFloor();
	DrawMesh();

	glUseProgram(0);
	//reset colors to be drawn, and viewport to window size
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	//unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"),0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"bumpTex"),1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"shadowTex"),2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"),1,(float*)&camera->GetPosition());

	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	//viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	UpdateShaderMatrices();

	DrawFloor();
	DrawMesh();

	glUseProgram(0);
}

void Renderer::DrawMesh() {
	modelMatrix.ToIdentity();
	SetShaderLight(*light);
	
	//textureMatrix = biasMatrix*(projMatrix*viewMatrix);
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),"textureMatrix"),1,false,*&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),"modelMatrix"),1,false,*&modelMatrix.values);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"),1, (float*)&camera->GetPosition());

	hellNode->Draw(*this);
}

void Renderer::DrawFloor() {
	modelMatrix.ToIdentity();
	/*modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(450, 450, 1));*/
	Matrix4 tempMatrix = textureMatrix * modelMatrix;


	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),"textureMatrix"),1,false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	floor->Draw();
}