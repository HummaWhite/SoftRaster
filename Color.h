#include <Windows.h>
#include <iostream>
#include <cmath>

#include "math/Vector.h"

struct RGB24
{
	RGB24() {}
	RGB24(BYTE _r, BYTE _g, BYTE _b): r(_r), g(_g), b(_b) {}

	RGB24(Vec3 color)
	{
		color *= 255;
		*this = RGB24(color[0], color[1], color[2]);
	}
	
	BYTE r, g, b;
};

struct RGBA32
{
	BYTE r, g, b, a;
};
