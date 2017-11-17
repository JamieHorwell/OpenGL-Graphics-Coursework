#include "Renderer.h"


Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();
	fps = 0;
	time = 0;
	skyBox = Mesh::GenerateQuad();
	waterMesh = new HeightMap();
	snowMountain = new HeightMap(TEXTUREDIR"snowMountain.raw");
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	//add shaders and textures to our resourceManager
	resources.addShader("basicShader", "TexturedVertex.glsl", "TexturedFragment.glsl");
	resources.addShader("reflectShader", "FluidVertex.glsl", "reflectFragment.glsl");
	resources.addShader("skyboxShader", "skyboxVertex.glsl", "skyboxFragment.glsl");
	resources.addShader("mountainBlend", "BumpVertexPerlin.glsl", "mountainFragmentPerlin.glsl");



	//Scene1 mesh + other setup
	waterQuad = Mesh::GenerateQuad();
	reflectionTest = Mesh::GenerateQuad();
	refractionTest = Mesh::GenerateQuad();
	portalQuad = new SceneNode(Mesh::GenerateQuad());
	portalQuad->SetModelScale(Vector3(200,200,1));
	portalQuad->SetTransform(Matrix4::Translation(Vector3(3700,540,5400)));
	reflectManager = new WaterReflectRefract(this);
	reflectManager->createWaterBuffers();
	
	//Scene2 mesh + other setup
	hellMountain = new HeightMap(TEXTUREDIR"landTest.raw");

	//Scene1 textures
	resources.addTexture("mountainSide.png");
	resources.addTexture("mountainSideBumpNew.png");
	resources.addTexture("snow7_d.jpg");
	resources.addTexture("snow7_local.jpg");
	resources.addSkybox("snowSky","stormydays_bk.tga","stormydays_ft.tga","stormydays_up.tga","stormydays_dn.tga","stormydays_lf.tga","stormydays_rt.tga");
	resources.addSkybox("hellSky","rusted_west.jpg","rusted_east.jpg","rusted_up.jpg","rusted_down.jpg","rusted_south.jpg","rusted_north.jpg");

	//Scene2 mesh + other setup
	portalQuad2 = new SceneNode(Mesh::GenerateQuad());
	portalQuad2->SetModelScale(Vector3(200,200,1));

	//Scene2 textures
	resources.addTexture("lava.png");
	resources.addTexture("Barren Reds.jpg");
	resources.addTexture("Barren RedsDOT3.JPG");
	resources.addTexture("hellMountainTop.JPG");

	//perlin texture to sample gradients from
	resources.addTexture("noiseSampler.png");
	SetTextureRepeating(resources.getTexture("noiseSampler.png"), true);



	//set some of our textures to be repeating
	SetTextureRepeating(resources.getTexture("mountainSide.png"), TRUE);
	SetTextureRepeating(resources.getTexture("mountainSideBumpNew.png"), TRUE);
	SetTextureRepeating(resources.getTexture("snow7_d.jpg"), TRUE);
	SetTextureRepeating(resources.getTexture("snow7_local.jpg"), TRUE);
	SetTextureRepeating(resources.getTexture("lava.png"), TRUE);

	//set our textures
	snowMountain->SetTexture(resources.getTexture("mountainSide.png"));
	snowMountain->SetBumpMap(resources.getTexture("mountainSideBumpNew.png"));
	portalQuad->GetMesh()->SetTexture(resources.getTexture("lava.png"));
	hellMountain->SetTexture(resources.getTexture("Barren Reds.jpg"));
	
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	mainLight = new Light(Vector3(3000, 8500.0f, 3000), Vector4(1,1,1,1), 50000);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer() {

}

void Renderer::RenderScene() {
	RenderScene1();
	RenderScene2();
	RenderScene3();
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	time += msec;
	fps = 1 / (msec / 1000);
}

void Renderer::RenderScene1()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//DRAW OUR SCENE FROM REFLECTION PERSPECTIVE
	glEnable(GL_CLIP_DISTANCE0);
	reflectManager->cameraReflectionPos(camera, viewMatrix);
	reflectManager->setPlaneToClip(150, true);
	UpdateShaderMatrices();
	bindFramebuffer(reflectManager->getReflectionFBO());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	RenderSkyBox(resources.getSkybox("snowSky"));
	RenderHeightMap(snowMountain);
	RenderPortal();

	//DRAW OUR SCENE FROM REFRACTION PERSPECTIVE
	reflectManager->resetCamera(camera, viewMatrix);
	reflectManager->setPlaneToClip(150, false);
	UpdateShaderMatrices();
	bindFramebuffer(reflectManager->getRefractionFBO());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	RenderSkyBox(resources.getSkybox("snowSky"));
	RenderHeightMap(snowMountain);

	//DRAW OUR SCENE NORMALLY
	glDisable(GL_CLIP_DISTANCE0);
	reflectManager->setPlaneToClip(1500000, true);
	bindScreenbuffer();
	RenderSkyBox(resources.getSkybox("snowSky"));
	RenderHeightMap(snowMountain);
	RenderPortal();
	RenderWater();
	DrawTextOrth(std::to_string(fps), Vector3(0, 0, 0));
	SwapBuffers();
}

void Renderer::RenderScene2()
{
	RenderSkyBox(resources.getSkybox("hellSky"));
	RenderHeightMap(hellMountain);
}

void Renderer::RenderScene3()
{
}

void Renderer::RenderSkyBox(GLuint skyboxTex)
{
	//dont want to write to depth buffer
	glDepthMask(GL_FALSE);
	SetCurrentShader(resources.getShader("skyboxShader"));

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP,skyboxTex);

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
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "reflectionTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "refractionTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "perlinTex"), 3);


	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, reflectManager->getReflectionTexture());
	//SetTextureRepeating(reflectionTexture, true);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, reflectManager->getRefractionTexture());
	//SetTextureRepeating(refractionTexture, true);



	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, resources.getTexture("noiseSampler.png"));


	float heightX = (RAW_WIDTH*HEIGHTMAP_X / 2.0f);
	float heightY = 150;
	float heightZ = (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f);
	

	modelMatrix = Matrix4::Translation(Vector3(0, 150, 0));
	
	
	UpdateShaderMatrices();

	waterMesh->Draw();


	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Renderer::RenderHeightMap(HeightMap* heightMap)
{
	SetCurrentShader(resources.getShader("mountainBlend"));
	SetShaderLight(*mainLight);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTexBump"), 1);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTexBump"), 3);

	//also pass in perlin texture
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "perlinTex"), 4);

	//pass in clipping plane
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "clippingPlane"), 1, (float*)&reflectManager->getPlaneToClip());

	//top of mountain texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,resources.getTexture("snow7_d.jpg"));
	//also bind normal for toptexture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, resources.getTexture("snow7_local.jpg"));

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, resources.getTexture("noiseSampler.png"));

	//reset matrices
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();


	UpdateShaderMatrices();

	heightMap->Draw();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

}

void Renderer::RenderPortal()
{
	SetCurrentShader(resources.getShader("basicShader"));
	SetShaderLight(*mainLight);

	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalQuad->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));

	portalQuad->Draw();

	glUseProgram(0);
	UpdateShaderMatrices();
}

//Debugging relfection and refraction textures
void Renderer::RenderfboTest()
{
	SetCurrentShader(resources.getShader("basicShader"));

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	reflectionTest->SetTexture(reflectManager->getReflectionTexture());

	

	modelMatrix = Matrix4::Translation(Vector3(1000, 350, 0)) * Matrix4::Scale(Vector3(1000,1000,1000));
	UpdateShaderMatrices();

	reflectionTest->Draw();


	

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	refractionTest->SetTexture(reflectManager->getRefractionTexture());

	modelMatrix = Matrix4::Translation(Vector3(3000, 350, 0)) * Matrix4::Scale(Vector3(1000, 1000, 1000));
	UpdateShaderMatrices();
	refractionTest->Draw();

	glUseProgram(0);
}


void Renderer::DrawTextOrth(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	SetCurrentShader(resources.getShader("basicShader"));
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *font);

	modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	Matrix4 projMatTemp = projMatrix;
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
	projMatrix = projMatTemp;
	glUseProgram(0);
}

void Renderer::createFBO(GLuint &FBOID)
{
	//generate our buffer
	glGenFramebuffers(1, &FBOID);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOID);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void Renderer::createTexture(GLuint & TexID)
{
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexID, 0);
}

void Renderer::createDepthTexture(GLuint & TexID)
{
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TexID, 0);
}

void Renderer::createDepthBufferAttachment(GLuint & depthBufferID)
{
	glGenRenderbuffers(1, &depthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
}

void Renderer::bindFramebuffer(GLuint fboID)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void Renderer::bindScreenbuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


