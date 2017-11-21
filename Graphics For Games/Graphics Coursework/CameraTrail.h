#pragma once
#include "../../nclgl/Camera.h"

struct trailNode {
	Vector3 nodePos;
	float pitch;
	float yaw;
	trailNode(Vector3 pos, float p, float y) : nodePos(pos), pitch(p), yaw(y) {}
};

class CameraTrail
{
public:
	CameraTrail(Camera* cam);
	~CameraTrail();


	void Update(float msec);

	void moveCam(float msec);

	void SetDirectionVector(int index);

protected:
	Camera* cam;

	std::vector<trailNode*> cameraTrail;

	Vector3 dirVector;

	float nodeDist;
	float currentDist;

	float speed;
	int currentNode;
};

