#pragma once
#include "SceneNode.h"
#include "OBJMesh.h"
class CubeRobot :
	public SceneNode
{
public:
	CubeRobot();
	~CubeRobot();

	virtual void Update(float msec);

	static void CreateCube() {
		OBJMesh * m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		cube = m;
		OBJMesh* headObj = new OBJMesh();
		headObj->LoadOBJMesh(MESHDIR"corsucant_01.obj");
		headMesh = headObj;
	}
	static void DeleteCube() { delete cube; };

protected:
	static Mesh* cube;
	static Mesh* headMesh;
	SceneNode* head;
	SceneNode*leftArm;
	SceneNode* rightArm;
};

