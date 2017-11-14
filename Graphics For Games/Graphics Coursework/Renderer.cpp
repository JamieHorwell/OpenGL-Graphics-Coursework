#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();

	skyBox = Mesh::GenerateQuad();


	//add shaders and textures to our resourceManager
	resources.addShader("reflectShader","PerPixelVertex.glsl","reflectFragment.glsl");
	resources.addShader("skyboxShader","skyboxVertex.glsl","skyboxFragment.glsl");
	resources.addShader("skyboxShader", "skyboxVertex.glsl", "skyboxFragment.glsl");
	resources.addShader("mountainBlend","PerPixelVertex.glsl","mountainFragment.glsl");

	//hell scene textures
	resources.addTexture("lava.png");
	resources.addTexture("Barren Reds.jpg");
	resources.addTexture("Barren RedsDOT3.JPG");
	resources.addTexture("hellMountainTop.JPG");
	
	//snow scene textures
	resources.addTexture("snow7_d.jpg");
	resources.addTexture("snow7_local.jpg");
	resources.addSkybox("snowSky","negx.jpg","posx.jpg","posy.jpg","negy.jpg","negz.jpg","posz.jpg");

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}


void Renderer::RenderScene() {

}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderSkyBox()
{
	//dont want to write to depth buffer
	glDepthMask(GL_FALSE);
	SetCurrentShader(resources.getShader("skyboxShader"));

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, resources.getSkybox("snowSky"));

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);
}
