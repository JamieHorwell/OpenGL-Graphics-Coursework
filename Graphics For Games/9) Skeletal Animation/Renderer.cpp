#include "Renderer.h"



Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera(0, -90.0f, Vector3(-180, 60, 0));

#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader = new Shader("skeletonvertex.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl", SHADERDIR"NormalExplosion.glsl");
#endif

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");

	hellNode = new MD5Node(*hellData);

	if (!currentShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_NONE);

	init = true;
}

Renderer::~Renderer(void) {
	delete camera;

	delete hellData;
	delete hellNode;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	hellNode->Update(msec);
	time += msec/ 300;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	modelMatrix = Matrix4::Scale(Vector3(10,10,10));
	UpdateShaderMatrices();

			UpdateShaderMatrices();
			hellNode->Draw(*this);
	

	glUseProgram(0);
	SwapBuffers();
}