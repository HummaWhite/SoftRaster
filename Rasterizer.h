#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <vector>
#include <cstdlib>
#include <thread>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"
#include "PipelineData.h"
#include "LineDrawer.h"

enum
{
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK
} CullFaceMode;

class Rasterizer
{
public:
	template<typename VertexData>
	static std::vector<VertexData> rasterize(
			std::vector<VertexData>& vertexData,
			int cullFaceMode)
	{
		std::vector<VertexData> outData;

		int triangleCount = vertexData.size() / 3;

		const int maxThreads = std::thread::hardware_concurrency();

		std::vector<VertexData> triangles[maxThreads];
		std::thread threads[maxThreads];

		for (int i = 0; i < maxThreads; i++)
		{
			int start = (triangleCount / maxThreads) * i;
			int end = std::min(triangleCount, (triangleCount / maxThreads) * (i + 1));

			threads[i] = std::thread
			(
				processTriangles<VertexData>,
				std::ref(triangles[i]),
				std::ref(vertexData),
				start, end,
				cullFaceMode
			);
		}

		for (auto& thread : threads)
		{
			thread.join();
		}

		for (int i = 0; i < maxThreads; i++)
		{
			outData.insert(outData.end(), triangles[i].begin(), triangles[i].end());
		}

		return outData;
	}

private:
	template<typename VertexData>
	static void processTriangles(
		std::vector<VertexData>& outData,
		std::vector<VertexData>& vertexData,
		int start,
		int end,
		int cullFaceMode)
	{
		for (register int i = start; i < end; i++)
		{
			VertexData va = vertexData[i * 3 + 0];
			VertexData vb = vertexData[i * 3 + 1];
			VertexData vc = vertexData[i * 3 + 2];

			if (cullFaceMode)
			{
				float coef = cullFaceMode == CULL_BACK ? 1.0f : -1.0f;
				if (coef * cross(Vec2{ float(vc.x - va.x), float(vc.y - va.y) }, Vec2{ float(vb.x - va.x), float(vb.y - va.y) }) > 0.0f) continue;
			}

			std::vector<VertexData> triangle = processTriangle(va, vb, vc);

			outData.insert(outData.end(), triangle.begin(), triangle.end());
		}
	}


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

		if (equals(va, vb, eps) && equals(vb, vc, eps)) return Vec3(1.0f) / 3.0f;
		if (equals(va, vb, eps)) return Vec3{ (vc - p).length() / 2.0f, (vc - p).length() / 2.0f, (p - va).length() } / ((vc - va).length() + eps);
		if (equals(vb, vc, eps)) return Vec3{ (va - p).length() / 2.0f, (va - p).length() / 2.0f, (p - vb).length() } / ((va - vb).length() + eps);
		if (equals(vc, va, eps)) return Vec3{ (vb - p).length() / 2.0f, (vb - p).length() / 2.0f, (p - vc).length() } / ((vb - vc).length() + eps);

		float area = cross(vc - va, vb - va);

		if (abs(area) <= eps)
		{
			return Vec3(1.0f) / 3.0f;
		}

		float la = cross(vc - p, vb - p) / area;
		float lb = cross(va - p, vc - p) / area;
		float lc = cross(vb - p, va - p) / area;

		return { la, lb, lc };
	}
};

#endif
