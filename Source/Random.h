#pragma once

/*! returns random float between [0,1] */
inline float RndFloat() 
{
	//rand() = between 0 and RAND_MAX (32767)
	return (float)(rand()) / (float)RAND_MAX;
}

/*! returns random float between [a,b] */
inline float RndFloat(float a, float b)
{
	return a + RndFloat() * (b - a);
}