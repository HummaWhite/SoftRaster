#ifndef VERTEXPROCESSOR_H
#define VERTEXPROCESSOR_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"

class VertexProcessor
{
public:
	template<typename Shader>
	static void processVertex(
			Buffer<typename Shader::VertexOut>& vertexOut,
			Buffer<typename Shader::VertexIn>& vertexIn,
			Shader& shader,
			int primitiveType,
			Buffer<UINT> *indices = nullptr)
	{
		int vertexCount = (indices == nullptr) ? vertexIn.count : indices->count;
		Buffer<typename Shader::VertexOut> clipSpaceVertexData(vertexCount);

		for (int i = 0; i < vertexCount; i++)
		{
			UINT index = (indices == nullptr) ? i : (*indices)[i];
			
			typename Shader::VertexOut tmp = shader.process(vertexIn[index]);
			clipSpaceVertexData[i] = tmp;
		}

		switch (primitiveType)
		{
			case Primitive::LINE:
				break;
			case Primitive::TRIANGLE:
				doClipping(vertexOut, clipSpaceVertexData);
				break;
			case Primitive::POINT:
			default:
				break;
		};

		for (int i = 0; i < vertexOut.count; i++)
		{
			vertexOut[i].sr_Position /= vertexOut[i].sr_Position[3];
		}
	}

private:
	template<typename VertexOut>
	static void doClipping(
			Buffer<VertexOut>& outData,
			Buffer<VertexOut>& clipSpaceVertexData)
	{
		int triangleCount = clipSpaceVertexData.count / 3;
		std::vector<VertexOut> clipResult;

		for (int i = 0; i < triangleCount; i++)
		{
			VertexOut va = clipSpaceVertexData[i * 3 + 0];
			VertexOut vb = clipSpaceVertexData[i * 3 + 1];
			VertexOut vc = clipSpaceVertexData[i * 3 + 2];

			std::vector<VertexOut> clipped = clipTriangle(va, vb, vc);

			clipResult.insert(clipResult.end(), clipped.begin(), clipped.end());
		}

		outData.init(clipResult.size());
		outData.load((void*)&clipResult[0], clipResult.size() * sizeof(VertexOut));
	}

	template<typename VertexOut>
	static std::vector<VertexOut> clipTriangle(
			VertexOut& v0,
			VertexOut& v1,
			VertexOut& v2)
	{
		std::vector<VertexOut> output = { v0, v1, v2 };

		if (areaCode(v0.sr_Position) == INSIDE 
				&& areaCode(v1.sr_Position) == INSIDE
				&& areaCode(v2.sr_Position) == INSIDE)
		{
			return output;
		}

		for (int i = 0; i < 6; i++)
		{
			std::vector<VertexOut> input(output);
			output.clear();

			for (int j = 0; j < input.size(); j++)
			{
				VertexOut va = input[j];
				VertexOut vb = input[(j + 1) % input.size()];

				if (inside(vb.sr_Position, planes[i]))
				{
					if (!inside(va.sr_Position, planes[i]))
					{
						output.push_back(intersect(va, vb, planes[i]));
					}
					output.push_back(vb);
				}
				else if (inside(va.sr_Position, planes[i]))
				{
					output.push_back(intersect(va, vb, planes[i]));
				}
			}
		}

		std::vector<VertexOut> res;
		for (int i = 1; i < output.size() - 1; i++)
		{
			res.push_back(output[0]);
			res.push_back(output[i]);
			res.push_back(output[i + 1]);
		}

		return res;
	}

	static bool inside(Vec4 plane, Vec4 pos)
	{
		return dot(plane, pos) >= 0.0f;
	}

	template<typename VertexOut>
	static VertexOut intersect(VertexOut& va, VertexOut& vb, Vec4 plane)
	{
		float da = dot(va.sr_Position, plane);
		float db = dot(vb.sr_Position, plane);

		float weight = da / (da - db);
		return VertexOut(va, vb, weight);
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
	const static std::vector<Vec4> planes;

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

const std::vector<Vec4> VertexProcessor::planes =
{
	{ 0.0f, 0.0f, 1.0f,  1.0f },
	{ 0.0f, 0.0f,-1.0f,  1.0f },
	{ 1.0f, 0.0f, 0.0f,  1.0f },
	{ 0.0f, 1.0f, 0.0f,  1.0f },
	{-1.0f, 0.0f, 0.0f,  1.0f },
	{ 0.0f,-1.0f, 0.0f,  1.0f }
};

#endif
