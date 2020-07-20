#include <iostream>

#include "acllib.h"
#include "FrameBuffer.h"
#include "Color.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "Vertex.h"
#include "VertexProcessor.h"

const int W_WIDTH = 1280;
const int W_HEIGHT = 720;

void draw(int x1, int y1, int x2, int y2, FrameBuffer<RGB24>& buffer)
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

	float data[] =
	{
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 4.0f,
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-10.0f, 5.0f, 2.0f,
		3.0f, -4.0f, 1.2f,
		0.0f, -4.89f, 4.2f
	};

	Buffer<SimpleShader::VertexIn> vb(9);
	vb.load(data, sizeof(data));

	SimpleShader shader;

	Mat4 model(1.0f);
	//model = translate(model, { 1.0f, -2.0f, 3.0f });

	shader.model = model;
	shader.view = lookAt({ 0.0f, -5.0f, 1.0f }, { 0.0, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });
	shader.proj = perspective(75.0f, (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);

	Buffer<SimpleShader::VertexOut> out;
	VertexProcessor::processVertex(out, vb, shader, Primitive::TRIANGLE);

	std::cout << std::endl;
	for (int i = 0; i < out.count; i++)
	{
		out[i].sr_Position.print();
	}

	FrameBuffer<RGB24> colorBuffer(W_WIDTH, W_HEIGHT);
	colorBuffer.fill({ 0, 0, 0 });

	FrameBuffer<float> depthBuffer(W_WIDTH, W_HEIGHT);
	depthBuffer.fill(0.0f);

	for (int i = 0; i < out.count / 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int x0 = (out[i * 3 + 0].sr_Position[0] + 1.0f) * W_WIDTH / 2.0f;
			int y0 = (out[i * 3 + 0].sr_Position[1] + 1.0f) * W_HEIGHT / 2.0f;

			int x1 = (out[i * 3 + 1].sr_Position[0] + 1.0f) * W_WIDTH / 2.0f;
			int y1 = (out[i * 3 + 1].sr_Position[1] + 1.0f) * W_HEIGHT / 2.0f;

			int x2 = (out[i * 3 + 2].sr_Position[0] + 1.0f) * W_WIDTH / 2.0f;
			int y2 = (out[i * 3 + 2].sr_Position[1] + 1.0f) * W_HEIGHT / 2.0f;

			draw(x0, y0, x1, y1, colorBuffer);
			draw(x1, y1, x2, y2, colorBuffer);
			draw(x0, y0, x2, y2, colorBuffer);
		}
	}

	flushScreen((BYTE*)colorBuffer.bufPtr(), W_WIDTH, W_HEIGHT);
}
