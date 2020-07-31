

#ifndef VERTEXPROCESSOR_H
#define VERTEXPROCESSOR_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"
#include "PipelineData.h"

const float CLIP_NEARPLANE_EPS = 1e-10;

class VertexProcessor
{
public:
	template<typename Shader>
	static std::vector<Pipeline::FSIn<typename Shader::VSToFS>> processVertex(
			std::vector<typename Shader::VSIn>& vertexIn,
			Shader& shader,
			Vec2 viewportSize,
			int primitiveType,
			std::vector<UINT> *indices = nullptr)
	{
		std::vector<Pipeline::FSIn<typename Shader::VSToFS>> outData;
		std::vector<Pipeline::VSOut<typename Shader::VSToFS>> clipped;
		std::vector<Pipeline::VSOut<typename Shader::VSToFS>> clipSpaceData;

		int vertexCount = (indices == nullptr) ? vertexIn.size() : indices->size();

		for (int i = 0; i < vertexCount; i++)
		{
			UINT index = (indices == nullptr) ? i : (*indices)[i];
			clipSpaceData.push_back(shader.processVertex(vertexIn[index]));
		}

		/*std::cout << "Input:\n";
		for (int i = 0; i < vertexCount; i++)
		{
			clipSpaceData[i].sr_Position.print();
		}*/

		switch (primitiveType)
		{
			case Primitive::LINE:
				break;
			case Primitive::TRIANGLE:
				clipped = doClipping(clipSpaceData);
				break;
			case Primitive::POINT:
			default:
				break;
		};

		//std::cout << "Output  " << clipped.size() << "\n";

		for (int i = 0; i < clipped.size(); i++)
		{
			Vec4& pos = clipped[i].sr_Position;
			pos[0] /= pos[3], pos[1] /= pos[3], pos[2] /= pos[3];
			
			int x = ((pos[0] + 1.0f) / 2.0f) * viewportSize[0];
			int y = ((pos[1] + 1.0f) / 2.0f) * viewportSize[1];

			pos[3] = std::max(pos[3], CLIP_NEARPLANE_EPS);

			// 注意：这里tmp.z存的是透视除法后（即NDC）的z值，而tmp.w存的是透视除法前（即CVV中）z值的倒数，用于透视校正插值
			Pipeline::FSIn<typename Shader::VSToFS> tmp;
			tmp.data = clipped[i].data;
			tmp.x = x, tmp.y = y, tmp.z = pos[2], tmp.w = 1.0f / pos[3];
			outData.push_back(tmp);
		}

		return outData;
	}

private:
	template<typename VSToFS>
	static std::vector<Pipeline::VSOut<VSToFS>> doClipping(
			std::vector<Pipeline::VSOut<VSToFS>>& clipSpaceData)
	{
		int triangleCount = clipSpaceData.size() / 3;
		std::vector<Pipeline::VSOut<VSToFS>> clipResult;

		for (int i = 0; i < triangleCount; i++)
		{
			Pipeline::VSOut<VSToFS> va = clipSpaceData[i * 3 + 0];
			Pipeline::VSOut<VSToFS> vb = clipSpaceData[i * 3 + 1];
			Pipeline::VSOut<VSToFS> vc = clipSpaceData[i * 3 + 2];

			std::vector<Pipeline::VSOut<VSToFS>> clipped = clipTriangle(va, vb, vc);

			clipResult.insert(clipResult.end(), clipped.begin(), clipped.end());
		}

		return clipResult;
	}

	template<typename VSToFS>
	static std::vector<Pipeline::VSOut<VSToFS>> clipTriangle(
			Pipeline::VSOut<VSToFS>& v0,
			Pipeline::VSOut<VSToFS>& v1,
			Pipeline::VSOut<VSToFS>& v2)
	{
		std::vector<Pipeline::VSOut<VSToFS>> output = { v0, v1, v2 };

		if (areaCode(v0.sr_Position) == INSIDE 
				&& areaCode(v1.sr_Position) == INSIDE
				&& areaCode(v2.sr_Position) == INSIDE)
		{
			return output;
		}

		for (int i = 0; i < 6; i++)
		{
			std::vector<Pipeline::VSOut<VSToFS>> input(output);
			output.clear();

			for (int j = 0; j < input.size(); j++)
			{
				Pipeline::VSOut<VSToFS> va = input[j];
				Pipeline::VSOut<VSToFS> vb = input[(j + 1) % input.size()];

				if (inside(vb.sr_Position, planeNorms[i]))
				{
					if (!inside(va.sr_Position, planeNorms[i]))
					{
						output.push_back(intersect(va, vb, planeNorms[i]));
					}
					output.push_back(vb);
				}
				else if (inside(va.sr_Position, planeNorms[i]))
				{
					output.push_back(intersect(va, vb, planeNorms[i]));
				}
			}
		}

		std::vector<Pipeline::VSOut<VSToFS>> res;

		for (int i = 1; i + 1 < output.size(); i++)
		{
			res.push_back(output[0]);
			res.push_back(output[i]);
			res.push_back(output[i + 1]);
		}

		return res;
	}

	static bool inside(Vec4 plane, Vec4 pos)
	{
		return dot(plane, pos) > 0.0f;
	}

	template<typename VSToFS>
	static Pipeline::VSOut<VSToFS> intersect(
			Pipeline::VSOut<VSToFS>& va,
			Pipeline::VSOut<VSToFS>& vb,
			Vec4 plane)
	{
		float da = dot(va.sr_Position, plane);
		float db = dot(vb.sr_Position, plane);

		float weight = (da - CLIP_NEARPLANE_EPS) / (da - db);
		Pipeline::VSOut<VSToFS> out(va, vb, weight);
		return out;
	}

	static int areaCode(Vec4 pos)
	{
		int code = INSIDE;

		if (pos[0] < -pos[3]) code |= LEFT;
		if (pos[0] > +pos[3]) code |= RIGHT;

		if (pos[1] < -pos[3]) code |= BOTTOM;
		if (pos[1] > +pos[3]) code |= TOP;

		if (pos[2] < -pos[3]) code |= ZNEAR;
		if (pos[2] > +pos[3]) code |= ZFAR;

		return code;
	}

private:
	const static std::vector<Vec4> planeNorms;

	enum
	{
		INSIDE	= 0,
		LEFT 	= 1 << 0,
		RIGHT	= 1 << 1,
		BOTTOM	= 1 << 2,
		TOP		= 1 << 3,
		ZNEAR	= 1 << 4,
		ZFAR	= 1 << 5
	} ClipAreaCode;
};

const std::vector<Vec4> VertexProcessor::planeNorms =
{
	{ 0.0f, 0.0f, 1.0f,  0.0f },
	{ 0.0f, 0.0f,-1.0f,  1.0f },
	{ 1.0f, 0.0f, 0.0f,  1.0f },
	{ 0.0f, 1.0f, 0.0f,  1.0f },
	{-1.0f, 0.0f, 0.0f,  1.0f },
	{ 0.0f,-1.0f, 0.0f,  1.0f }
};

#endif
