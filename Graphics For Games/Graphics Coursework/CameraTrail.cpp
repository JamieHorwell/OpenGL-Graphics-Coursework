#include "CameraTrail.h"



CameraTrail::CameraTrail(Camera* cam)
{
	this->cam = cam;
	currentNode = -1;
	cameraTrail.push_back(new trailNode(Vector3(-770, 2200, -500), -26.4, 230.8));
	cameraTrail.push_back(new trailNode(Vector3(3900, 1500, 600), -11.55, 176));
	cameraTrail.push_back(new trailNode(Vector3(7310, 590, 3050), -15.4, 127));
	cameraTrail.push_back(new trailNode(Vector3(7275, 686, 7223), -9.94, 74.8));
	cameraTrail.push_back(new trailNode(Vector3(3500, 492, 7558), -1.6, 356));
	cameraTrail.push_back(new trailNode(Vector3(3580, 492, 6300), -2.17, 358));

	speed = 400;
	cam->SetPosition(cameraTrail[0]->nodePos);
	cam->SetPitch(cameraTrail[0]->pitch);
	cam->SetYaw(cameraTrail[0]->yaw);
}


CameraTrail::~CameraTrail()
{
}

void CameraTrail::Update(float msec)
{
	currentDist = (cam->GetPosition() - cameraTrail[currentNode + 1]->nodePos).Length();
	if (currentDist < 5) {
		if (currentNode != cameraTrail.size() - 2) {
			currentNode++;
			SetDirectionVector(currentNode);
			nodeDist = (cameraTrail[currentNode + 1]->nodePos - cameraTrail[currentNode]->nodePos).Length();
			cam->SetPosition(cameraTrail[currentNode]->nodePos);
			cam->SetPitch(cameraTrail[currentNode]->pitch);
			cam->SetYaw(cameraTrail[currentNode]->yaw);
		}
	}
	else {
		moveCam(msec);
	}
	//move cams position
	

}

void CameraTrail::moveCam(float msec)
{
	cam->SetPosition(cam->GetPosition() + dirVector * (speed * (msec / 1000)));
	float traveledFrac = (nodeDist - currentDist) / nodeDist;
	cam->SetPitch(cameraTrail[currentNode]->pitch + (cameraTrail[currentNode + 1]->pitch - cameraTrail[currentNode]->pitch)*traveledFrac);
	cam->SetYaw(cameraTrail[currentNode]->yaw + (cameraTrail[currentNode + 1]->yaw - cameraTrail[currentNode]->yaw)*traveledFrac);
}

void CameraTrail::SetDirectionVector(int index)
{
	dirVector = cameraTrail[index + 1]->nodePos - cameraTrail[index]->nodePos;
	dirVector.Normalise();
}
