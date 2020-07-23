#ifndef SHADER_H
#define SHADER_H

#include "math/Vector.h"
#include "math/Matrix.h"
#include "PipelineData.h"
#include "FrameBufferAdapter.h"

struct SimpleShader
{
	// VertexShader到FragmentShader之间传递的数据类型
	struct VSToFS
	{
		VSToFS() {}
		VSToFS(VSToFS& from, VSToFS& to, float weight)
		{
			//裁剪阶段插值
			//通过构造函数生成插值后的实例
			color = lerp(from.color, to.color, weight);
		}

		VSToFS(VSToFS& va, VSToFS& vb, VSToFS& vc, Vec3 weight)
		{
			//光栅化阶段的三角形重心插值，这里所有z都为倒数
			color = triLerp(va.color, vb.color, vc.color, weight);
		}

		Vec4 color;
	};

	struct VSIn
	{
		Vec3 pos;
		Vec3 color;
	};

	Pipeline::VSOut<VSToFS> processVertex(VSIn in)
	{
		Pipeline::VSOut<VSToFS> out;

		Vec4 inPos = { in.pos[0], in.pos[1], in.pos[2], 1.0f };

		out.sr_Position = proj * view * model * inPos;
		out.data.color = { in.color[0], in.color[1], in.color[2], 1.0f };
		return out;
	}

	void processFragment(FrameBufferAdapter& adapter, Pipeline::FSIn<VSToFS> in)
	{
		Vec3 color(in.data.color);
		adapter.writeColor(0, in.x, in.y, color);
		adapter.writeDepth(in.x, in.y, in.z);
	}

	Mat4 model;
	Mat4 view;
	Mat4 proj;
};

#endif
