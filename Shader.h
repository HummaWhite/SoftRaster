#ifndef SHADER_H
#define SHADER_H

#include "math/Vector.h"
#include "math/Matrix.h"

struct SimpleShader
{
	struct VertexOut
	{
		VertexOut() {}
		VertexOut(VertexOut& from, VertexOut& to, float weight)
		{
			//通过构造函数生成插值后的实例
			sr_Position = lerp(from.sr_Position, to.sr_Position, weight);
		}

		Vec4 sr_Position;
	};

	struct VertexIn
	{
		Vec3 pos;
	};

	VertexOut process(VertexIn in)
	{
		Vec4 inPos = { in.pos[0], in.pos[1], in.pos[2], 1.0f };
		VertexOut out;
		out.sr_Position = proj * model * view * inPos;
		return out;
	}

	Mat4 model;
	Mat4 view;
	Mat4 proj;
};

#endif
