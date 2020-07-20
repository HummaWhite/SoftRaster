#ifndef SRMATH_H
#define SRMATH_H

const float Pi = 3.1415926535897932;
const float EPS = 1e-6;

float toRad(float deg)
{
	return deg * Pi / 180.0f;
}

float toDeg(float rad)
{
	return rad * 180.0f / Pi;
}

template<typename T>
T lerp(T from, T to, float weight)
{
	return from + (to - from) * weight;
}

#endif
