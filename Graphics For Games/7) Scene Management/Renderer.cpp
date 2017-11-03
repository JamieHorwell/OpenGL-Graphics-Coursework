#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera = new Camera();
	camera->SetPosition(Vector3(0,100,750.0f));

	currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,0));

	if (!currentShader->LinkProgram() || !quad->GetTexture()) {
		return;
	}

	root = new SceneNode();

	for (int i = 0; i < 5; ++i) {
		SceneNode * s = new SceneNode();
		s->SetColour(Vector4(1.0f,1.0f,1.0f,0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(0,100.0f,-300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f,100.0f,100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(quad);
		root->AddChild(s);
	}

	root->AddChild(new CubeRobot());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

Renderer::~Renderer(void) {
	delete root;
	delete quad;
	delete camera;
	CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix*viewMatrix);
	root->Update(msec);
}