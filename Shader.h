#ifndef SHADER_H
#define SHADER_H

#include "math/Vector.h"
#include "math/Matrix.h"
#include "PipelineData.h"
#include "FrameBufferAdapter.h"

struct SimpleShader
{
	// VS到FS之间传递的数据类型，目前只能做到手动给每一个数据插值
	struct VSToFS
	{
		VSToFS() {}
		VSToFS(VSToFS& from, VSToFS& to, float weight)
		{
			//裁剪阶段插值
			//通过构造函数生成插值后的实例
			pos = lerp(from.pos, to.pos, weight);
			texCoord = lerp(from.texCoord, to.texCoord, weight);
			norm = lerp(from.norm, to.norm, weight);
		}

		VSToFS(VSToFS& va, VSToFS& vb, VSToFS& vc, Vec3 weight)
		{
			//光栅化阶段的三角形重心插值
			pos = triLerp(va.pos, vb.pos, vc.pos, weight);
			texCoord = triLerp(va.texCoord, vb.texCoord, vc.texCoord, weight);
			norm = triLerp(va.norm, vb.norm, vc.norm, weight);
		}

		Vec3 pos;
		Vec2 texCoord;
		Vec3 norm;
	};

	// 输入VS的数据类型
	struct VSIn
	{
		Vec3 pos;
		Vec2 texCoord;
		Vec3 norm;
	};

	// Vertex Shader
	Pipeline::VSOut<VSToFS> processVertex(VSIn in)
	{
		Pipeline::VSOut<VSToFS> out;

		Vec4 inPos = { in.pos[0], in.pos[1], in.pos[2], 1.0f };
		out.sr_Position = proj * view * model * inPos;

		Vec4 outPos = model * inPos;

		Mat3 m(model);
		Mat3 modelInv = inverse(m).transpose();

		out.data.pos = { outPos[0], outPos[1], outPos[2] };
		out.data.texCoord = in.texCoord;
		out.data.norm = (modelInv * in.norm).normalized();
		return out;
	}

	// Fragment Shader
	void processFragment(FrameBufferAdapter& adapter, Pipeline::FSIn<VSToFS> in)
	{
		Vec3 result(0.0f);

		Vec3 L = { 0.0f, 0.0f, -1.0f };
		L = L.normalized();
		Vec3 N = in.data.norm;

		float strength = std::max(0.0f, dot(-L, N)) + 0.05f;

		result = Vec3(strength);

		adapter.writeColor(0, result);
		adapter.writeDepth(in.z);
	}

	//uniforms
	Mat4 model;
	Mat4 view;
	Mat4 proj;
};

#endif
