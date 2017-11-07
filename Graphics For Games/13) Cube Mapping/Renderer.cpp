#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	quad = Mesh::GenerateQuad();

	camera->SetPosition(Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f,500.0f,RAW_WIDTH*HEIGHTMAP_X));

	light = new Light(Vector3((RAW_HEIGHT*HEIGHTMAP_TEX_X / 2.0f), 500.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)), Vector4(1, 1, 1, 1), (RAW_WIDTH*HEIGHTMAP_TEX_X) / 2.0f);
	light->SetRadius(2000);

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",SHADERDIR"reflectFragment.glsl");

	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",SHADERDIR"skyboxFragment.glsl");

	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl",SHADERDIR"PerPixelFragment.glsl");

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram()) {
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.bmp",TEXTUREDIR"rusted_east.bmp",TEXTUREDIR"rusted_up.bmp",TEXTUREDIR"rusted_down.bmp",TEXTUREDIR"rusted_south.bmp",TEXTUREDIR"rusted_north.bmp",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
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
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightmap();
	DrawWater();

	SwapBuffers();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"),1,(float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"),0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 0);

}