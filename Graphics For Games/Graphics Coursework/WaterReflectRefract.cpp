#include "WaterReflectRefract.h"



WaterReflectRefract::WaterReflectRefract()
{
}


WaterReflectRefract::~WaterReflectRefract()
{
}

void WaterReflectRefract::setPlaneToClip(float height)
{
	planeToClip = Vector4(0, -1, 0, height);
	glEnable(GL_CLIP_DISTANCE0);
}
