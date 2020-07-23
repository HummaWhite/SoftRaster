#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"
#include "PipelineData.h"

class Rasterizer
{
public:
	template<typename VertexData>
	static std::vector<VertexData> rasterize(
			std::vector<VertexData>& vertexData)
	{
		std::vector<VertexData> outData;

		int triangleCount = vertexData.size() / 3;

		for (int i = 0; i < triangleCount; i++)
		{
			VertexData va = vertexData[i * 3 + 0];
			VertexData vb = vertexData[i * 3 + 1];
			VertexData vc = vertexData[i * 3 + 2];

			std::vector<VertexData> triangle = processTriangle(va, vb, vc);

			outData.insert(outData.end(), triangle.begin(), triangle.end());
		}

		return outData;
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
				if (sorted[j].y > sorted[j + 1].y)
				{
					std::swap(sorted[j], sorted[j + 1]);
				}
			}
		}

		float x0 = sorted[0].x, y0 = sorted[0].y;
		float x1 = sorted[1].x, y1 = sorted[1].y;
		float x2 = sorted[2].x, y2 = sorted[2].y;

		//扫描线光栅化，生成片段
		for (int i = y0; i <= y2; i++)
		{
			int endX = (y2 == y0) ? x2 : lerp<float>(x0, x2, float(i - y0) / float(y2 - y0));
			int startX = x1;

			if (i == y0) startX = (y1 == y0) ? x1 : x0;
			else if (i < y1) startX = lerp<float>(x0, x1, float(i - y0) / float(y1 - y0));
			else if (i < y2) startX = lerp<float>(x1, x2, float(i - y1) / float(y2 - y1));
			else if (i == y2) startX = (y1 == y2) ? x1 : x2;

			//std::cout << i << "  " << startX << "  " << endX << std::endl;
			int dx = (endX > startX) ? 1 : -1;
			//startX -= dx, endX += dx;

			for (int j = startX; j != endX; j += dx)
			{
				Vec2 p = { (float)j, (float)i };
				Vec2 va = { (float)sorted[0].x, (float)sorted[0].y };
				Vec2 vb = { (float)sorted[1].x, (float)sorted[1].y };
				Vec2 vc = { (float)sorted[2].x, (float)sorted[2].y };

				float area = abs(cross(vc - va, vb - va));
				float la = abs(cross(vb - p, vc - p)) / area;
				float lb = abs(cross(vc - p, va - p)) / area;
				float lc = abs(cross(va - p, vb - p)) / area;

				Vec3 weight = { la, lb, lc };

				VertexData fragment(sorted[0], sorted[1], sorted[2], weight);
				fragment.x = j;
				fragment.y = i;
				fragment.z = 1.0f / fragment.z;

				output.push_back(fragment);
			}
		}
		return output;
	}
};

#endif
