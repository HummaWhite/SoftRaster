#ifndef COLOR_H
#define COLOR_H

#include <Windows.h>
#include <iostream>
#include <cmath>

#include "math/Vector.h"

struct RGB24
{
	RGB24() {}
	RGB24(BYTE _r, BYTE _g, BYTE _b):
		r(_r), g(_g), b(_b) 
	{
		r = std::min(255, std::max(0, (int)r));
		g = std::min(255, std::max(0, (int)g));
		b = std::min(255, std::max(0, (int)b));
	}

	RGB24(Vec3 color)
	{
		Vec3 c = color;
		c[0] = std::min(1.0f, std::max(0.0f, c[0]));
		c[1] = std::min(1.0f, std::max(0.0f, c[1]));
		c[2] = std::min(1.0f, std::max(0.0f, c[2]));
		c *= 255;
		*this = RGB24(c[0], c[1], c[2]);
	}

	Vec3 toVec3()
	{
		return Vec3{ (float)r, (float)g, (float)b } / 255.0f;
	}

	Vec4 toVec4()
	{
		return Vec4{ (float)r, (float)g, (float)b, 0.0f } / 255.0f;
	}
	
	BYTE r, g, b;
};

struct RGBA32
{
	BYTE r, g, b, a;
};

#endif
