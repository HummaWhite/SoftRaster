#ifndef SRMATH_H
#define SRMATH_H

#include <cstdlib>
#include <cstring>

const float Pi = 3.1415926535897932;
const float EPS = 1e-6;

static float toRad(float deg)
{
	return deg * Pi / 180.0f;
}

static float toDeg(float rad)
{
	return rad * 180.0f / Pi;
}

#endif
