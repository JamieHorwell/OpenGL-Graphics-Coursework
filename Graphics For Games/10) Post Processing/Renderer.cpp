#include "Renderer.h"



Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	camera = new Camera(0.0f,135.0f,Vector3(0,500,0));

	quad = Mesh::GenerateQuad();

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS));

	sceneShader = new Shader(SHADERDIR"TexturedVertex.glsl",SHADERDIR"TexturedFragment.glsl");

	processShader = new Shader(SHADERDIR"TexturedVertex.glsl",SHADERDIR"blurFrag.glsl");

	if (!processShader->LinkProgram() || !sceneShader->LinkProgram() || !heightMap->GetTexture()) {
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(),true);

	//generate 3 textures, depth texture, two colour textures to use as attachments for our FBOs
	glGenTextures(1, &bufferDepthTex);

	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	//clamp tex coords
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//minified and magnified texture settings
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//generate the data to fill the texture - refer to Appendix A tutorial 3 for loading a texture with data in memory rather than from file
	glTexImage2D(GL_TEXTURE_2D,0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//minified and magnified texture settings
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		
	}

	//read scene into this
	glGenFramebuffers(1, &bufferFBO);
	//do post processing in this buffer
	glGenFramebuffers(1, &processFBO);

	//bind bufferFBO, attach textures to it to render into
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0] || !bufferColourTex[1]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}


Renderer::~Renderer()
{
	delete sceneShader;
	delete processShader;
	currentShader = NULL;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);

	delete heightMap;
	delete quad;
	delete camera;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	DrawScene();
	DrawPostProcess();
	PresentScene();
	SwapBuffers();
}

//to use post processing using FBOS, render the scene into a texture
void Renderer::DrawScene() {
	//heightmape will now be drawn to buffer FBO rather than screen, into its first colour attachment: bufferColourTex[0]
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	//clear whatever is in bufferFBO
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f,10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess() {
	//bind out processFBO to draw into
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	//attach bufferColourTex[1] to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	//clear FBO
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(sceneShader);
	//orthographic matrix from -1.0 to 1.0 on each axis
	projMatrix = Matrix4::Orthographic(-1,1,1,-1,-1,1);
	//make view matrix identity matrix as our quad will perfectly fill screen space
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	//will be drawing quad multiple times, so disable depth testing, make sure quad always drawn
	glDisable(GL_DEPTH_TEST);

	//get texel size, as tex coords from 0.0 to 1.0, so divide 1.0 by width and height
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(),"pixelSize"), 1.0f / width, 1.0f / height);

	// 2 pass gaussian blur, both passes performed by same shader, switched between axis by isVertical 
	for (int i = 0; i < POST_PASSES; ++i) {
		//bind bufferColourTex[1] as colour attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1],0);


		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"),0);
		quad->SetTexture(bufferColourTex[0]);
		quad->Draw();
		//swap colour buffers and do second blur pass on opposite axis
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Orthographic(-1,1,1,-1,-1,1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();
	glUseProgram(0);

}
