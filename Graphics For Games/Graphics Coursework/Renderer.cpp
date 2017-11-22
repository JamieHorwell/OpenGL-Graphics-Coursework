#include "Renderer.h"


Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();
	camTrail = new CameraTrail(camera);
	fps = 0;
	time = 0;
	sceneOn = SceneRender::Scene1;
	skyBox = Mesh::GenerateQuad();
	waterMesh = new HeightMap();
	snowMountain = new HeightMap(TEXTUREDIR"snowMountain.raw");
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	//add shaders and textures to our resourceManager
	resources.addShader("basicShader", "TexturedVertex.glsl", "TexturedFragment.glsl");
	resources.addShader("particleShader","ParticleVertex.glsl", "particleFragment.glsl");
	resources.addShader("reflectShader", "FluidVertex.glsl", "reflectFragment.glsl");
	resources.addShader("lavaShader", "lavaVertex.glsl", "lavaFragment.glsl");
	resources.addShader("skyboxShader", "skyboxVertex.glsl", "skyboxFragment.glsl");
	resources.addShader("mountainBlend", "mountainVertex.glsl", "mountainFragmentPerlin.glsl");
	resources.addShader("shadowShader","shadowVert.glsl", "shadowFrag.glsl");

	//Scene Independent setup
	shadowMan = new shadowManager(this);
	postProcesser = new PostProcessing(this);
	postProcess = false;

	//Scene1 mesh + other setup
	reflectionTest = Mesh::GenerateQuad();
	refractionTest = Mesh::GenerateQuad();
	portalQuad = new SceneNode(Mesh::GenerateQuad());
	portalQuad->SetModelScale(Vector3(200,300,1));
	portalQuad->SetTransform(Matrix4::Translation(Vector3(3700,540,5400)));
	reflectManager = new WaterReflectRefract(this);
	reflectManager->createWaterBuffers();
	

	//Scene1 textures
	resources.addTexture("mountainSide.png");
	resources.addTexture("mountainSideBumpNew.png");
	resources.addTexture("snow7_d.jpg");
	resources.addTexture("snow7_local.jpg");
	resources.addSkybox("snowSky","stormydays_bk.tga","stormydays_ft.tga","stormydays_up.tga","stormydays_dn.tga","stormydays_lf.tga","stormydays_rt.tga");
	resources.addSkybox("hellSky","rusted_west.jpg","rusted_east.jpg","rusted_up.jpg","rusted_down.jpg","rusted_south.jpg","rusted_north.jpg");

	

	//Scene2 textures
	resources.addTexture("lava.png");
	resources.addTexture("lavaBump.png");
	resources.addTexture("Barren Reds.jpg");
	resources.addTexture("Barren RedsDOT3.JPG");
	resources.addTexture("hellMountainTop.JPG");
	resources.addTexture("HellMountainBump.png");
	resources.addTexture("eruptionParticle.JPG");

	//Scene2 mesh + other setup
	hellMountain = new HeightMap(TEXTUREDIR"landTest.raw");
	particleEmitter = new ParticleEmitter(this, resources.getShader("particleShader"), resources.getTexture("eruptionParticle.JPG"), 5000, 100, -300, 6000, Vector3(2100,1900,7000), Vector3(15,15,15));
	portalQuad2 = new SceneNode(Mesh::GenerateQuad());
	portalQuad2->SetModelScale(Vector3(200, 300, 1));
	portalQuad2->SetTransform(Matrix4::Translation(Vector3(4700, 2200, 5200)));
	portalQuad2->SetTransform(portalQuad2->GetTransform());
	portal = new Portal(this);


	//perlin texture to sample gradients from
	resources.addTexture("noiseSampler.png");
	SetTextureRepeating(resources.getTexture("noiseSampler.png"), true);



	//set some of our textures to be repeating
	SetTextureRepeating(resources.getTexture("mountainSide.png"), TRUE);
	SetTextureRepeating(resources.getTexture("mountainSideBumpNew.png"), TRUE);
	SetTextureRepeating(resources.getTexture("hellMountainTop.JPG"), TRUE);
	SetTextureRepeating(resources.getTexture("HellMountainBump.png"), TRUE);
	SetTextureRepeating(resources.getTexture("Barren Reds.jpg"), TRUE);
	SetTextureRepeating(resources.getTexture("Barren RedsDOT3.JPG"), TRUE);
	SetTextureRepeating(resources.getTexture("snow7_d.jpg"), TRUE);
	SetTextureRepeating(resources.getTexture("snow7_local.jpg"), TRUE);
	SetTextureRepeating(resources.getTexture("lava.png"), TRUE);

	//set our textures
	snowMountain->SetTexture(resources.getTexture("mountainSide.png"));
	snowMountain->SetBumpMap(resources.getTexture("mountainSideBumpNew.png"));
	snowMountain->SetTopTex(resources.getTexture("snow7_d.jpg"));
	snowMountain->SetTopTexBump(resources.getTexture("snow7_local.jpg"));
	hellMountain->SetTexture(resources.getTexture("Barren Reds.jpg"));
	hellMountain->SetBumpMap(resources.getTexture("Barren RedsDOT3.JPG"));
	hellMountain->SetTopTex(resources.getTexture("hellMountainTop.JPG"));
	hellMountain->SetTopTexBump(resources.getTexture("HellMountainBump.png"));

	waterMesh->SetTexture(resources.getTexture("lava.png"));
	waterMesh->SetBumpMap(resources.getTexture("lavaBump.png"));
	//debugging textures for portals
	portalQuad->GetMesh()->SetTexture(resources.getTexture("lava.png"));
	portalQuad2->GetMesh()->SetTexture(resources.getTexture("lava.png"));
	hellMountain->SetTexture(resources.getTexture("Barren Reds.jpg"));
	
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	mainLight = new Light(Vector3(1000, 5400.0f, 1000), Vector4(1,1,1,1), 50000000);

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
	switch (sceneOn) {
		case SceneRender::Scene1:
			RenderScene1(true);
			break;
		case SceneRender::Scene2:
			RenderScene2(true);
			break;
		case SceneRender::Scene3:

			break;
	}
	DrawTextOrth(std::to_string(fps), Vector3(0, 0, 0), 15);
	std::string pos = "x:" + to_string((camera->GetPosition().x)) + "  y:" + to_string((camera->GetPosition().y)) + "   z" + to_string((camera->GetPosition().z));
	DrawTextOrth(pos, Vector3(0, 20, 0), 15);
	std::string rot = "pitch:" + to_string(camera->GetPitch()) + "  yaw:" + to_string(camera->GetYaw());
	DrawTextOrth(rot, Vector3(0,40,0), 15);
	SwapBuffers();
}

void Renderer::UpdateScene(float msec) {
	///////SCENE INDEPENDENT UPDATING///////
	camera->SetPrevPos(camera->GetPosition());
	//camTrail->Update(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	time += msec;
	fps = 1 / (msec / 1000);

	//////SCENE SPECIFIC UPDATING////////
	switch (sceneOn) {
	case SceneRender::Scene1:
		if (portal->portal_intersection(camera->getPrevPos(), camera->GetPosition(), portalQuad, msec)) {
			Vector4 newCamPos = Matrix4::Inverse(portal->getPortalView(viewMatrix,portalQuad,portalQuad2)) * Vector4(0,0,0,1);
			//extract rotation from viewmatrix too?
			camera->SetPosition(Vector3(newCamPos.x,newCamPos.y,newCamPos.z));
			sceneOn = SceneRender::Scene2;
		}
		break;
	case SceneRender::Scene2:
		//check for other portal intersection
		particleEmitter->updateParticles(msec);
		postProcess = true;
		if (portal->portal_intersection(camera->getPrevPos(), camera->GetPosition(), portalQuad2, msec)) {
			Vector4 newCamPos = Matrix4::Inverse(portal->getPortalView(viewMatrix, portalQuad2, portalQuad)) * Vector4(0, 0, 0, 1);
			//extract rotation from viewmatrix too?
			camera->SetPosition(Vector3(newCamPos.x, newCamPos.y, newCamPos.z));
			sceneOn = SceneRender::Scene1;
			
		}
		break;
	case SceneRender::Scene3:

		break;
	}
	
	
}

void Renderer::RenderScene1(bool renderPortal, bool shadowPersp)
{
	
	if (!shadowPersp && renderPortal) {
		//DRAW OUR SCENE FROM REFLECTION PERSPECTIVE
		glEnable(GL_CLIP_DISTANCE0);
		if (renderPortal) reflectManager->cameraReflectionPos(*camera, viewMatrix);
		reflectManager->setPlaneToClip(150, true);
		bindFramebuffer(reflectManager->getReflectionFBO());
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RenderSkyBox(resources.getSkybox("snowSky"));
		if (renderPortal) portal->renderFromPortalView(portalQuad, portalQuad2, true, SceneRender::Scene2);
		RenderHeightMap(snowMountain);
	}

	if (!shadowPersp && renderPortal) {
		//DRAW OUR SCENE FROM REFRACTION PERSPECTIVE
		if (renderPortal) reflectManager->resetCamera(*camera, viewMatrix);
		reflectManager->setPlaneToClip(150, false);
		UpdateShaderMatrices();
		bindFramebuffer(reflectManager->getRefractionFBO());
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RenderSkyBox(resources.getSkybox("snowSky"));
		RenderHeightMap(snowMountain);
	}
	
	//DRAW OUR SCENE NORMALLY
	glDisable(GL_CLIP_DISTANCE0);
	reflectManager->setPlaneToClip(1500000, true);
	if(!shadowPersp) bindScreenbuffer();
	if (!shadowPersp && renderPortal) shadowMan->DrawShadowScene(SceneRender::Scene1);
	if(!shadowPersp)RenderSkyBox(resources.getSkybox("snowSky"));
	if (renderPortal) portal->renderFromPortalView(portalQuad,portalQuad2, false, SceneRender::Scene2);
	RenderHeightMap(snowMountain, shadowPersp);
	if(!shadowPersp)RenderWater();
	if(shadowPersp)RenderPortal(portalQuad,shadowPersp);
	
}

void Renderer::RenderScene2(bool renderPortal, bool shadowPersp)
{
	if(!shadowPersp)RenderSkyBox(resources.getSkybox("hellSky"));
	if (!shadowPersp && renderPortal) shadowMan->DrawShadowScene(SceneRender::Scene2);
	if(renderPortal) portal->renderFromPortalView(portalQuad2,portalQuad, false, SceneRender::Scene1);
	RenderHeightMap(hellMountain, shadowPersp);
	if(!shadowPersp)renderLava();
	if(!shadowPersp)particleEmitter->renderParticles();
	
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

void Renderer::RenderHeightMap(HeightMap* heightMap, bool shadowPersp)
{
	if (shadowPersp) {
		SetCurrentShader(resources.getShader("shadowShader"));
	}
	else {
		SetCurrentShader(resources.getShader("mountainBlend"));
		UpdateShaderMatrices();
	}
	
	SetShaderLight(*mainLight);


	/********BINDING SHADER ATTRIBS**********/
	
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	if (!shadowPersp) {
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTexBump"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTex"), 2);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "topTexBump"), 3);
		//also pass in perlin texture
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "perlinTex"), 4);
		//pass in shadowTex
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 5);
		//pass in clipping plane
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "clippingPlane"), 1, (float*)&reflectManager->getPlaneToClip());
	}
	

	/********ACTIVATING TEXTURE UNITS**********/
	if (!shadowPersp) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap->GetTopTex());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, heightMap->GetTopTexBump());
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, resources.getTexture("noiseSampler.png"));
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shadowMan->getShadowTex());
	}
	
	//reset matrices
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	textureMatrix.ToIdentity();
	Matrix4 tempMatrix = shadowMan->getShadowTexMatrix() * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	if (shadowPersp) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, *&shadowMan->getLightViewMatrix().values);
	}
	
	

	heightMap->Draw();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

}

void Renderer::RenderPortal(SceneNode* portalToRender,bool shadowPersp)
{
	if (shadowPersp) {
		SetCurrentShader(resources.getShader("shadowShader"));
	}
	else {
		SetCurrentShader(resources.getShader("basicShader"));
	}
	
	SetShaderLight(*mainLight);

	UpdateShaderMatrices();
	Matrix4 tempMatrix = shadowMan->getShadowTexMatrix() * (portalToRender->GetTransform()*Matrix4::Scale(portalToRender->GetModelScale()));
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	if(shadowPersp) glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, *&shadowMan->getLightViewMatrix().values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalToRender->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));

	portalToRender->Draw();


	glUseProgram(0);
	
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
	textureMatrix.ToIdentity();
	Matrix4 projMatTemp = projMatrix;
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
	projMatrix = projMatTemp;
	glUseProgram(0);
}

void Renderer::renderLava()
{
	SetCurrentShader(resources.getShader("lavaShader"));
	SetShaderLight(*mainLight);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "perlinTex"), 2);
	modelMatrix = Matrix4::Translation(Vector3(0, 150, 0));
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, resources.getTexture("noiseSampler.png"));


	waterMesh->Draw();
	modelMatrix = Matrix4::Translation(Vector3(0, 150, 0));

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Renderer::createFBO(GLuint &FBOID, bool colorAttch, bool depthAttach, bool stencilAttch)
{
	//generate our buffer
	glGenFramebuffers(1, &FBOID);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOID);
	if(colorAttch) glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if(depthAttach) glDrawBuffer(GL_DEPTH_ATTACHMENT);
	if(stencilAttch) glDrawBuffer(GL_STENCIL_ATTACHMENT);
	if (!colorAttch && !depthAttach && !stencilAttch) glDrawBuffer(GL_NONE);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, width, height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TexID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, TexID, 0);
}

void Renderer::createDepthBufferAttachment(GLuint & depthBufferID)
{
	glGenRenderbuffers(1, &depthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
}

void Renderer::createStencilTexture(GLuint & TexID)
{
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_COMPONENTS, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
}





void Renderer::bindFramebuffer(GLuint fboID)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::bindScreenbuffer()
{
	if (postProcess) {
		glBindFramebuffer(GL_FRAMEBUFFER, postProcesser->getbufferFBO());
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
}


