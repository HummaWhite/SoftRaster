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

int Setup()
{
	initWindow("Test", DEFAULT, DEFAULT, W_WIDTH, W_HEIGHT);

	float data[] =
	{
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		-10.0f, 5.0f, 2.0f,
		3.0f, -4.0f, 1.2f,
		0.0f, 3.5f, 4.2f
	};

	Buffer<SimpleShader::InType> vb(6);
	vb.load(data, sizeof(data));

	SimpleShader shader;

	Mat4 model(1.0f);
	//model = translate(model, { 1.0f, -2.0f, 3.0f });

	shader.model = model;
	shader.view = lookAt({ 0.0f, -5.0f, 0.0f }, { 0.0, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });
	shader.proj = perspective(75.0f, (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);

	Buffer<SimpleShader::OutType> out;
	VertexProcessor::processVertex(out, vb, shader, Primitive::TRIANGLE);

	for (int i = 0; i < out.count; i++)
	{
		out[i].sr_Position.print();
	}

	FrameBuffer<RGB24> colorBuffer(W_WIDTH, W_HEIGHT);
	colorBuffer.fill({ 0, 0, 0 });

	FrameBuffer<float> depthBuffer(W_WIDTH, W_HEIGHT);
	depthBuffer.fill(0.0f);

	for (int i = 0; i < out.count; i++)
	{
		int x = (out[i].sr_Position[0] + 1.0f) * W_WIDTH / 2.0f;
		int y = (out[i].sr_Position[1] + 1.0f) * W_HEIGHT / 2.0f;

		if (x >= W_WIDTH || x < 0 || y >= W_HEIGHT || y < 0) continue;

		colorBuffer(x, y) = { 255, 255, 255 };
	}

	flushScreen((BYTE*)colorBuffer.bufPtr(), W_WIDTH, W_HEIGHT);
}
