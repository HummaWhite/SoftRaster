#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"

class Rasterizer
{
public:
	template<typename VertexData>
	static void rasterize(
			Buffer<VertexData>& outData,
			Buffer<VertexData>& vertexData)
	{
		int triangleCount = vertexData.count / 3;
		std::vector<VertexData> rasterized;

		for (int i = 0; i < triangleCount; i++)
		{
			VertexData va = vertexData[i * 3 + 0];
			VertexData vb = vertexData[i * 3 + 1];
			VertexData vc = vertexData[i * 3 + 2];

			std::vector<VertexData> triangle = processTriangle(va, vb, vc);

			rasterized.insert(rasterized.end(), triangle.begin(), triangle.end());
		}

		outData.init(rasterized.size());
		outData.load((void*)&rasterized[0], rasterized.size() * sizeof(VertexData));
	}

private:
	template<typename VertexData>
	static std::vector<VertexData> processTriangle(
			VertexData& v0,
			VertexData& v1,
			VertexData& v2)
	{
		std::vector<VertexData> output;

		VertexData sorted[] = { v0, v1, v2 };
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (sorted[j].sr_Position[1] > sorted[j + 1].sr_Position[1])
				{
					std::swap(sorted[j], sorted[j + 1]);
				}
			}
		}

		sorted[0].sr_Position.print();
		sorted[0].color.print();
		sorted[1].sr_Position.print();
		sorted[1].color.print();
		sorted[2].sr_Position.print();
		sorted[2].color.print();

		float x0 = sorted[0].sr_Position[0], y0 = sorted[0].sr_Position[1];
		float x1 = sorted[1].sr_Position[0], y1 = sorted[1].sr_Position[1];
		float x2 = sorted[2].sr_Position[0], y2 = sorted[2].sr_Position[1];

		//扫描线光栅化，生成片段
		for (int i = y0; i <= y2; i++)
		{
			int endX = lerp(x0, x2, (i - y0) / (y2 - y0));
			int startX = x1;

			if (i == y0) startX = (y1 == y0) ? x1 : x0;
			else if (i < y1) startX = lerp(x0, x1, (i - y0) / (y1 - y0));
			else if (i < y2) startX = lerp(x1, x2, (i - y1) / (y2 - y1));
			else if (i == y2) startX = (y1 == y2) ? x1 : x2;

			int dx = (endX > startX) ? 1 : -1;

			for (int j = startX; j != endX; j += dx)
			{
				Vec2 p = { (float)j, (float)i };
				Vec2 va(sorted[0].sr_Position);
				Vec2 vb(sorted[1].sr_Position);
				Vec2 vc(sorted[2].sr_Position);

				float area = abs(cross(vc - va, vb - va));
				float la = abs(cross(vb - p, vc - p)) / area;
				float lb = abs(cross(vc - p, va - p)) / area;
				float lc = abs(cross(va - p, vb - p)) / area;

				VertexData fragment(sorted[0], sorted[1], sorted[2], la, lb, lc);
				fragment.sr_Position[0] = j;
				fragment.sr_Position[1] = i;

				output.push_back(fragment);
			}
		}

		return output;
	}
};

#endif
