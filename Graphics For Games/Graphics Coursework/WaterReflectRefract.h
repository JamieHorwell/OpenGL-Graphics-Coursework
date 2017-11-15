#pragma once
#include "../../nclgl/OGLRenderer.h"
class WaterReflectRefract
{
public:
	WaterReflectRefract();
	~WaterReflectRefract();

	void setPlaneToClip(float height);


protected:
	Vector4 planeToClip;
	
};

