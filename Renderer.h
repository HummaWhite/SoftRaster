#ifndef RENDERER_H
#define RENDERER_H

#include "VertexProcessor.h"
#include "Rasterizer.h"
#include "FragmentProcessor.h"
#include "FrameBufferAdapter.h"
#include "PipelineData.h"

struct Renderer
{
	template<typename Shader>
	void draw(
			std::vector<typename Shader::VSIn>& vertexArray,
			Shader& shader,
			FrameBufferAdapter& adapter)
	{
		int width, height;

		if (adapter.colorAttachments.size() == 0)
		{
			if (adapter.depthAttachment == nullptr) return;
			else
			{
				width = adapter.depthAttachment->width();
				height = adapter.depthAttachment->height();
			}
		}
		else
		{
			width = adapter.colorAttachments[0]->width();
			height = adapter.colorAttachments[0]->height();
		}

		std::vector<Pipeline::FSIn<SimpleShader::VSToFS>> vertexOut = VertexProcessor::processVertex(vertexArray, shader, { (float)width, (float)height }, Primitive::TRIANGLE);

		if (renderMode < 2)
		{
			std::vector<Pipeline::FSIn<typename Shader::VSToFS>> fragments = Rasterizer::rasterize(vertexOut);
			FragmentProcessor::processFragment(adapter, shader, fragments);
		}

		if (renderMode != 0) drawFrame(vertexOut, adapter);
	}

	template<typename VertexData>
	void drawFrame(
			std::vector<VertexData>& vertexData,
			FrameBufferAdapter& adapter)
	{
		for (int i = 0; i < vertexData.size() / 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int x0 = vertexData[i * 3 + 0].x;
				int y0 = vertexData[i * 3 + 0].y;

				int x1 = vertexData[i * 3 + 1].x;
				int y1 = vertexData[i * 3 + 1].y;

				int x2 = vertexData[i * 3 + 2].x;
				int y2 = vertexData[i * 3 + 2].y;

				drawLine(x0, y0, x1, y1, *adapter.colorAttachments[0]);
				drawLine(x1, y1, x2, y2, *adapter.colorAttachments[0]);
				drawLine(x0, y0, x2, y2, *adapter.colorAttachments[0]);
			}
		}
	}

	void drawLine(int x1, int y1, int x2, int y2, FrameBufferDouble<RGB24>& buffer)
	{
		int width = buffer.width();
		int height = buffer.height();

		LineDrawer dw(x1, y1, x2, y2);

		while (!dw.finished())
		{
			int x = dw.x(), y = dw.y();

			if (x < width && x >= 0 && y < height && y >= 0)
			{
				buffer(x, height - y - 1) = { 255, 255, 255 };
			}
			dw.nextStep();
		}

		int x = dw.x(), y = dw.y();
		if (x < width && x >= 0 && y < height && y >= 0)
		{
			buffer(x, height - y - 1) = { 255, 255, 255 };
		}

		if (renderMode == 3) return;

		for (int i = x1 - 4; i <= x1 + 4; i++)
		{
			for (int j = y1 - 4; j <= y1 + 4; j++)
			{
				if (i >= width || i < 0 || j >= height || j < 0) continue;
				buffer(i, height - j - 1) = { 255, 0, 255 };
			}
		}

		for (int i = x2 - 4; i <= x2 + 4; i++)
		{
			for (int j = y2 - 4; j <= y2 + 4; j++)
			{
				if (i >= width || i < 0 || j >= height || j < 0) continue;
				buffer(i, height - j - 1) = { 255, 0, 255 };
			}
		}
	}

	int renderMode = 0;
};

#endif
