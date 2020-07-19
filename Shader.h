#ifndef SHADER_H
#define SHADER_H

#include "math/Vector.h"
#include "math/Matrix.h"

struct SimpleShader
{
	struct OutType
	{
		Vec4 sr_Position;
	};

	struct InType
	{
		Vec3 pos;
	};

	OutType process(InType in)
	{
		Vec4 inPos = { in.pos[0], in.pos[1], in.pos[2], 1.0f };
		OutType out;
		out.sr_Position = proj * model * view * inPos;
		return out;
	}

	Mat4 model;
	Mat4 view;
	Mat4 proj;
};

#endif
