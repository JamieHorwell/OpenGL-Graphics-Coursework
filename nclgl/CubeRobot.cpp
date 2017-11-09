#include "CubeRobot.h"

Mesh * CubeRobot::cube = NULL;
Mesh * CubeRobot::headMesh = NULL;
CubeRobot::CubeRobot()
{
	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	AddChild(body);

	head = new SceneNode(Mesh::GenerateQuad(), Vector4(0, 1, 0, 1));
	head->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"gurp.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	head->GetMesh()->GetTexture();

	head->SetModelScale(Vector3(18,18,18));
	head->SetTransform(Matrix4::Translation(Vector3(0,40,0)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0,0,1,1));
	leftArm->SetModelScale(Vector3(6,-18,6));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12,30,-1)));
	body->AddChild(leftArm);

	rightArm = new SceneNode(cube, Vector4(0,0,1,1));
	rightArm->SetModelScale(Vector3(6,-18,6));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12,30,-1)));
	body->AddChild(rightArm);

	SceneNode* leftLeg = new SceneNode(cube,Vector4(0,0,1,1));
	leftLeg->SetModelScale(Vector3(3,-17.5,3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->AddChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube,Vector4(0,0,1,1));
	rightLeg->SetModelScale(Vector3(3,-17.5,3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8,0,0)));
	body->AddChild(rightLeg);

	body->SetBoundingRadius(15.0f);
	head->SetBoundingRadius(5.0f);
	
	leftArm->SetBoundingRadius(18.0f);
	rightArm->SetBoundingRadius(18.0f);

	leftLeg->SetBoundingRadius(18.0f);
	rightLeg->SetBoundingRadius(18.0f);
}


CubeRobot::~CubeRobot()
{
}


void CubeRobot::Update(float msec) {
	transform = transform * Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));

	head->SetTransform(head->GetTransform()*Matrix4::Rotation(-msec / 10.0f,Vector3(0,0,0)));

	leftArm->SetTransform(leftArm->GetTransform()*Matrix4::Rotation(-msec / 2.0f,Vector3(1,0,0)));

	rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(msec / 2.0f, Vector3(1, 0, 0)));

	//derived from sceneNode, so call our update function
	SceneNode::Update(msec);
}