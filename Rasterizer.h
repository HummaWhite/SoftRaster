#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <vector>
#include <cstdlib>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"
#include "PipelineData.h"
#include "LineDrawer.h"

class Rasterizer
{
public:
	template<typename VertexData>
	static std::vector<VertexData> rasterize(
			std::vector<VertexData>& vertexData)
	{
		std::vector<VertexData> outData;

		int triangleCount = vertexData.size() / 3;

		for (register int i = 0; i < triangleCount; i++)
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
		int ty = y2, by = y0;

		// 扫描线光栅化，生成片段
		LineDrawer ab(x0, y0, x1, y1);
		LineDrawer bc(x1, y1, x2, y2);
		LineDrawer ac(x0, y0, x2, y2);
		LineDrawer* dw[] = { &ab, &bc, &ac };

		int sx[ty - by + 1], ex[ty - by + 1];

		memset(sx, 0x3f, sizeof(sx));
		memset(ex, 0x00, sizeof(ex));

		for (int i = 0; i < 3; i++)
		{
			while (!dw[i]->finished())
			{
				int x = dw[i]->x(), y = dw[i]->y();

				sx[y - by] = std::min(sx[y - by], x);
				ex[y - by] = std::max(ex[y - by], x);

				dw[i]->nextStep();
			}

			int x = dw[i]->x(), y = dw[i]->y();
			sx[y - by] = std::min(sx[y - by], x);
			ex[y - by] = std::max(ex[y - by], x);
		}

		Vec2 va = { x0, y0 };
		Vec2 vb = { x1, y1 };
		Vec2 vc = { x2, y2 };

		for (register int i = by; i <= ty; i++)
		{
			int l = sx[i - by], r = ex[i - by];

			for (int j = l; j <= r; j++)
			{
				Vec2 p = { (float)j, (float)i };
				Vec3 weight = getWeight(va, vb, vc, p);

				VertexData fragment(sorted[0], sorted[1], sorted[2], weight);
				fragment.x = j;
				fragment.y = i;

				output.push_back(fragment);
			}
		}
		return output;
	}

	static Vec3 getWeight(Vec2& va, Vec2& vb, Vec2& vc, Vec2& p)
	{
		const float eps = 1e-3;

		if (equals(va, vb, eps) && equals(vb, vc, eps)) return { 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f };
		if (equals(va, vb, eps)) return Vec3{ (vc - p).length() / 2.0f, (vc - p).length() / 2.0f, (p - va).length() } / ((vc - va).length() + eps);
		if (equals(vb, vc, eps)) return Vec3{ (va - p).length() / 2.0f, (va - p).length() / 2.0f, (p - vb).length() } / ((va - vb).length() + eps);
		if (equals(vc, va, eps)) return Vec3{ (vb - p).length() / 2.0f, (vb - p).length() / 2.0f, (p - vc).length() } / ((vb - vc).length() + eps);

		float area = cross(vc - va, vb - va);

		float la = cross(vc - p, vb - p) / area;
		float lb = cross(va - p, vc - p) / area;
		float lc = cross(vb - p, va - p) / area;

		return { la, lb, lc };
	}
};

#endif
