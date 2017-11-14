#include "PerlinNoise.h"



PerlinNoise::PerlinNoise()
{
}


PerlinNoise::~PerlinNoise()
{
}

float PerlinNoise::fade(float t)
{
	return t*t*t*(t*(6.0 - 15.0) + 10.0);
}
