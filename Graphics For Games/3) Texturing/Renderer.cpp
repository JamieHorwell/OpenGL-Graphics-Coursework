#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();
	cameraStart = camera->BuildViewMatrix();
	heightMap = new HeightMap(TEXTUREDIR"landTest.raw");
	quad = Mesh::GenerateQuad();

	camera->SetPosition(Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH*HEIGHTMAP_X));

	light = new Light(Vector3(3000, 1500.0f, 3000), Vector4(1, 1, 1, 1), (RAW_WIDTH*HEIGHTMAP_TEX_X) / 2.0f);
	light->SetRadius(120000);

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");

	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");

	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");

	mountainShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"mountainFragment.glsl");

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !mountainShader->LinkProgram()) {
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"lava.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	toptex = SOIL_load_OGL_texture(TEXTUREDIR"hellMountainTop.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap() || !toptex) {
		return;
	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	initPortal();
}

Renderer::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
	//moveLight();
}

void Renderer::renderFromPortalViews()
{

	//1. disable color and depth draw, enable writing to stencil buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);


	//2. stencil test always fails on every pixel drawn
	glStencilFunc(GL_NEVER, 0, 0xFF);

	//3. increment stencil value on stencil fail
	glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);  // increment stencil value on stencil fail


											 //4. draw our portal, which will fill inside of portal frame with 1s
	SetCurrentShader(lightShader);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalQuad->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));
	portalQuad->Draw();


	//5. now get viewmatrix from portal
	Matrix4 tempViewHolder = viewMatrix;
	viewMatrix = getPortalView(viewMatrix, portalQuad, portalQuad2);
	UpdateShaderMatrices();

	//6. enable color and depth drawing, disable writing to stencil buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0x00);

	//7. stencil function to GL_LEQUAL, only draw where stencil has been incremented, so where the portal frame is
	glStencilFunc(GL_LEQUAL, 1, 0xff);

	//8. draw rest of scene from portalView ( as weve changed viewmatrix in OGLRenderer
	//DrawSkybox();
	DrawHeightmap();
	//DrawPortal();
	DrawWater();

	//reset view 
	viewMatrix = tempViewHolder;

	//9. disable stencil test, color buffer drawing, enable depth drawing
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);

	//10. clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	//11. draw quad to depth buffer
	SetCurrentShader(lightShader);
	UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalQuad->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));
	portalQuad->Draw();
	glUseProgram(0);

	//12. enable color buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	UpdateShaderMatrices();

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawSkybox();
	renderFromPortalViews();

	//DrawSkybox();
	DrawHeightmap();
	DrawPortal();
	DrawWater();

	SwapBuffers();
}




void Renderer::DrawSkybox() {
	//dont want to write to depth buffer
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cubeMap);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::moveLight()
{
	this->light->SetPosition(light->GetPosition() + Vector3(10, 0, 10));
}

void Renderer::DrawPortal()
{
	SetCurrentShader(lightShader);
	SetShaderLight(*light);




	/*glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),"modelMatrix"), 1, false, (float*)&(portalQuad->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));

	portalQuad->Draw();*/


	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalQuad2->GetTransform()*Matrix4::Scale(portalQuad2->GetModelScale())));

	portalQuad2->Draw();

	glUseProgram(0);
	UpdateShaderMatrices();

}

Matrix4 Renderer::getPortalView(Matrix4 originalView, SceneNode * portalSrc, SceneNode * portalDest)
{
	//modelview matrix of portal
	Matrix4 mv = originalView * portalSrc->GetTransform();

	//need to invert portalDest
	Matrix4 portalCam = mv * Matrix4::Rotation(90, Vector3(0, 1, 0)) *  portalDest->GetTransform();

	return viewMatrix * Matrix4::Translation(Vector3(1000,1000,1000));
	//return cameraStart;
}

void Renderer::initPortal()
{
	portalQuad = new SceneNode(Mesh::GenerateQuad());
	portalQuad->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"lava.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	portalQuad->SetModelScale(Vector3(200, 200, 200));
	portalQuad->SetTransform(Matrix4::Translation(Vector3(1000, 1000, 1000)));

	portalQuad2 = new SceneNode(Mesh::GenerateQuad());
	portalQuad2->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"lava.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	portalQuad2->SetModelScale(Vector3(200, 200, 200));
	portalQuad2->SetTransform(Matrix4::Translation(Vector3(4500, 1000, 4500)));
	portalQuad2->SetTransform(portalQuad2->GetTransform() * Matrix4::Rotation(90, Vector3(0, 1, 0)));


}

void Renderer::DrawHeightmap() {
	SetCurrentShader(mountainShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, toptex);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}


void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH*HEIGHTMAP_X / 2.0f);
	float heightY = 200 * HEIGHTMAP_Y / 4.0f;
	float heightZ = (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f);


	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
}