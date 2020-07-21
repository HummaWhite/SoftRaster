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
			//裁剪阶段插值
			//通过构造函数生成插值后的实例
			color = lerp(from.color, to.color, weight);
		}

		VertexOut(VertexOut& va, VertexOut& vb, VertexOut& vc, float la, float lb, float lc)
		{
			//光栅化阶段的三角形重心插值，这里所有z都为倒数
			float za = va.sr_Position[2];
			float zb = vb.sr_Position[2];
			float zc = vc.sr_Position[2];
			float z = za * la + zb * lb + zc * lc;

			color = (va.color * za * la + vb.color * zb * lb + vc.color * zc * lc) / z;
		}

		Vec4 sr_Position;
		Vec4 color;
	};

	struct VertexIn
	{
		Vec3 pos;
		Vec3 color;
	};

	VertexOut processVertex(VertexIn in)
	{
		Vec4 inPos = { in.pos[0], in.pos[1], in.pos[2], 1.0f };
		VertexOut out;
		out.sr_Position = proj * model * view * inPos;
		out.color = { in.color[0], in.color[1], in.color[2], 1.0f };
		return out;
	}

	Mat4 model;
	Mat4 view;
	Mat4 proj;
};

#endif
