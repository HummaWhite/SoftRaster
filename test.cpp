#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>

#include "acllib.h"
#include "FrameBuffer.h"
#include "Color.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "Vertex.h"
#include "VertexProcessor.h"
#include "Rasterizer.h"
#include "Camera.h"
#include "PipelineData.h"
#include "FrameBufferAdapter.h"
#include "FragmentProcessor.h"

const int W_WIDTH = 1280;
const int W_HEIGHT = 720;

void draw(int x1, int y1, int x2, int y2, FrameBufferDouble<RGB24>& buffer)
{
	for (int i = x1 - 4; i <= x1 + 4; i++)
	{
		for (int j = y1 - 4; j <= y1 + 4; j++)
		{
			if (i >= W_WIDTH || i < 0 || j >= W_HEIGHT || j < 0) continue;
			buffer(i, W_HEIGHT - j) = { 255, 0, 255 };
		}
	}

	for (int i = x2 - 4; i <= x2 + 4; i++)
	{
		for (int j = y2 - 4; j <= y2 + 4; j++)
		{
			if (i >= W_WIDTH || i < 0 || j >= W_HEIGHT || j < 0) continue;
			buffer(i, W_HEIGHT - j) = { 255, 0, 255 };
		}
	}

	int stepX = (x1 < x2) ? 1 : -1;
	int stepY = (y1 < y2) ? 1 : -1;
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	if (dx > dy)
	{
		for (int x = x1, y = y1, eps = 0; x != x2; x += stepX)
		{
			eps += dy;
			if (eps * 2 >= dx)
			{
				y += stepY;
				eps -= dx;
			}

			if (x >= W_WIDTH || x < 0 || y >= W_HEIGHT || y < 0) continue;
			buffer(x, W_HEIGHT - y) = { 255, 255, 255 };
		}
	}
	else
	{
		for (int x = x1, y = y1, eps = 0; y != y2; y += stepY)
		{
			eps += dx;
			if (eps * 2 >= dy)
			{
				x += stepX;
				eps -= dy;
			}

			if (x >= W_WIDTH || x < 0 || y >= W_HEIGHT || y < 0) continue;
			buffer(x, W_HEIGHT - y) = { 255, 255, 255 };
		}
	}
}

int Setup()
{
	initWindow("Test", DEFAULT, DEFAULT, W_WIDTH, W_HEIGHT);

	std::vector<SimpleShader::VSIn> vb =
	{
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -1.0f, -2.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		/*{ { -10.0f, 5.0f, 2.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { 3.0f, -4.0f, 1.2f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.0f, -4.89f, 4.2f }, { 0.0f, 0.0f, 1.0f } },*/
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 1.0f } }
	};

	SimpleShader shader;

	Camera camera({ 0.0f, -5.0f, 2.0f });
	camera.setFOV(75.0f);

	Mat4 model(1.0f);

	FrameBufferDouble<RGB24> colorBuffer(W_WIDTH, W_HEIGHT);
	colorBuffer.fill({ 0, 0, 0 });

	FrameBufferDouble<float> depthBuffer(W_WIDTH, W_HEIGHT);
	depthBuffer.fill(1.0f);

	FrameBufferAdapter adapter;
	adapter.colorAttachments.push_back(&colorBuffer);
	adapter.depthAttachment = &depthBuffer;

	while (1)
	{
		colorBuffer.fill({ 0, 0, 0 });
		depthBuffer.fill(1.0f);

		model = rotate(model, { 1.0f, 0.0f, 1.0f }, 1.0f);

		shader.model = model;
		shader.view = camera.viewMatrix({ 0.0f, 0.0f, 0.0f });
		shader.proj = camera.projMatrix(W_WIDTH, W_HEIGHT);

		std::vector<Pipeline::FSIn<SimpleShader::VSToFS>> vertexOut = VertexProcessor::processVertex(vb, shader, { W_WIDTH, W_HEIGHT }, Primitive::TRIANGLE);

		/*std::cout << vertexOut.size() << std::endl;
		  for (int i = 0; i < vertexOut.size(); i++)
		  {
		  std::cout << vertexOut[i].x << " " << vertexOut[i].y << " " << vertexOut[i].z << std::endl;
		  }

		  std::cout << "\n";*/

		std::vector<Pipeline::FSIn<SimpleShader::VSToFS>> fragments = Rasterizer::rasterize(vertexOut);
		//std::cout << fragments.size() << std::endl;

		FragmentProcessor::processFragment(adapter, shader, fragments);

		/*for (int i = 0; i < vertexOut.size() / 3; i++)
		  {
		  for (int j = 0; j < 3; j++)
		  {
		  int x0 = vertexOut[i * 3 + 0].x;
		  int y0 = vertexOut[i * 3 + 0].y;

		  int x1 = vertexOut[i * 3 + 1].x;
		  int y1 = vertexOut[i * 3 + 1].y;

		  int x2 = vertexOut[i * 3 + 2].x;
		  int y2 = vertexOut[i * 3 + 2].y;

		  draw(x0, y0, x1, y1, colorBuffer);
		  draw(x1, y1, x2, y2, colorBuffer);
		  draw(x0, y0, x2, y2, colorBuffer);
		  }
		 }*/

		flushScreen((BYTE*)colorBuffer.getCurrentBuffer().bufPtr(), W_WIDTH, W_HEIGHT);
		colorBuffer.swap();
		depthBuffer.swap();
	}
}
