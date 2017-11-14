#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();

	skyBox = Mesh::GenerateQuad();
	waterMesh = Mesh::GenerateQuad();
	terrain = new HeightMap(TEXTUREDIR"snowMountain.raw");

	//add shaders and textures to our resourceManager
	resources.addShader("reflectShader","PerPixelVertex.glsl","reflectFragment.glsl");
	resources.addShader("skyboxShader","skyboxVertex.glsl","skyboxFragment.glsl");
	resources.addShader("mountainBlend","BumpVertex.glsl","mountainFragment.glsl");

	//hell scene textures
	resources.addTexture("lava.png");
	resources.addTexture("Barren Reds.jpg");
	resources.addTexture("Barren RedsDOT3.JPG");
	resources.addTexture("hellMountainTop.JPG");
	
	//snow scene textures
	resources.addTexture("mountainSide.png");
	resources.addTexture("mountainSideBumpNew.png");
	resources.addTexture("snow7_d.jpg");
	resources.addTexture("snow7_local.jpg");
	resources.addSkybox("snowSky","stormydays_bk.tga","stormydays_ft.tga","stormydays_up.tga","stormydays_dn.tga","stormydays_lf.tga","stormydays_rt.tga");
	resources.addSkybox("hellSky","rusted_west.jpg","rusted_east.jpg","rusted_up.jpg","rusted_down.jpg","rusted_south.jpg","rusted_north.jpg");

	//set some of our textures to be repeating
	SetTextureRepeating(resources.getTexture("mountainSide.png"), TRUE);
	SetTextureRepeating(resources.getTexture("mountainSideBumpNew.png"), TRUE);
	SetTextureRepeating(resources.getTexture("snow7_d.jpg"), TRUE);
	SetTextureRepeating(resources.getTexture("snow7_local.jpg"), TRUE);

	//set our textures
	waterMesh->SetTexture(resources.getTexture("lava.png"));
	terrain->SetTexture(resources.getTexture("mountainSide.png"));
	terrain->SetBumpMap(resources.getTexture("mountainSideBumpNew.png"));




	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	mainLight = new Light(Vector3(3000, 2500.0f, 3000), Vector4(1,1,1,1), 50000);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer::~Renderer() {

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	RenderSkyBox();
	//RenderWater();
	RenderHeightMap();
	SwapBuffers();
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
	glBindTexture(GL_TEXTURE_CUBE_MAP,resources.getSkybox("snowSky"));

	UpdateShaderMatrices();
	skyBox->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::RenderWater()
{
	SetCurrentShader(resources.getShader("reflectShader"));
	SetShaderLight(*mainLight);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, resources.getSkybox("snowSky"));

	waterMesh->Draw();

	glUseProgram(0);
}

void Renderer::RenderHeightMap()
{
	SetCurrentShader(resources.getShader("mountainBlend"));
	SetShaderLight(*mainLight);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTexBump"), 1);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTexBump"), 3);


	//top of mountain texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,resources.getTexture("snow7_d.jpg"));
	//also bind normal for toptexture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, resources.getTexture("snow7_local.jpg"));

	//reset matrices
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();


	UpdateShaderMatrices();

	terrain->Draw();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

}
