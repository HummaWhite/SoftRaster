#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>

#include "acllib.h"
#include "FrameBuffer.h"
#include "Color.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "VertexProcessor.h"
#include "Rasterizer.h"
#include "Camera.h"
#include "PipelineData.h"
#include "FrameBufferAdapter.h"
#include "FragmentProcessor.h"
#include "Shape.h"

const int W_WIDTH = 960;
const int W_HEIGHT = 540;

bool keyPressing[256] = { false };

void draw(int x1, int y1, int x2, int y2, FrameBufferDouble<RGB24>& buffer)
{
	for (int i = x1 - 4; i <= x1 + 4; i++)
	{
		for (int j = y1 - 4; j <= y1 + 4; j++)
		{
			if (i >= W_WIDTH || i < 0 || j >= W_HEIGHT || j < 0) continue;
			buffer(i, W_HEIGHT - j - 1) = { 255, 0, 255 };
		}
	}

	for (int i = x2 - 4; i <= x2 + 4; i++)
	{
		for (int j = y2 - 4; j <= y2 + 4; j++)
		{
			if (i >= W_WIDTH || i < 0 || j >= W_HEIGHT || j < 0) continue;
			buffer(i, W_HEIGHT - j - 1) = { 255, 0, 255 };
		}
	}

	LineDrawer dw(x1, y1, x2, y2);

	while (!dw.finished())
	{
		int x = dw.x(), y = dw.y();

		if (x < W_WIDTH && x >= 0 && y < W_HEIGHT && y >= 0)
		{
			buffer(x, W_HEIGHT - y - 1) = { 255, 255, 255 };
		}
		dw.nextStep();
	}

	int x = dw.x(), y = dw.y();
	if (x < W_WIDTH && x >= 0 && y < W_HEIGHT && y >= 0)
	{
		buffer(x, W_HEIGHT - y - 1) = { 255, 255, 255 };
	}
}

FrameBufferDouble<float> depthBuffer(W_WIDTH, W_HEIGHT);
FrameBufferDouble<RGB24> colorBuffer(W_WIDTH, W_HEIGHT);
Mat4 model(1.0f);
SimpleShader shader;
Camera camera({ 0.0f, -5.0f, 2.0f });
FrameBufferAdapter adapter;

std::vector<SimpleShader::VSIn> vb;

int renderMode = 0;

void drawLines(std::vector<Pipeline::FSIn<SimpleShader::VSToFS>>& vertexData)
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

			draw(x0, y0, x1, y1, colorBuffer);
			draw(x1, y1, x2, y2, colorBuffer);
			draw(x0, y0, x2, y2, colorBuffer);
		}
	}
}

bool F1Pressed = false;
bool cursorDisabled = false;

void processKey()
{
	if (keyPressing['W']) camera.move('W');
	if (keyPressing['S']) camera.move('S');
	if (keyPressing['A']) camera.move('A');
	if (keyPressing['D']) camera.move('D');
	if (keyPressing[VK_SHIFT]) camera.move(VK_SHIFT);
	if (keyPressing[VK_SPACE]) camera.move(VK_SPACE);
}

void render(int id)
{
	colorBuffer.fill({ 0, 0, 0 });;
	depthBuffer.fill(1.0f);

	if (!cursorDisabled) processKey();

	model = rotate(model, { 1.0f, 0.0f, 1.0f }, 1.0f);
	Mat3 m(model);
	m = inverse(m).transpose();

	shader.model = model;
	shader.view = camera.viewMatrix();
	shader.proj = camera.projMatrix(W_WIDTH, W_HEIGHT);
	shader.albedo = { 0.9f, 0.7f, 0.5f };
	shader.metallic = 1.0f;
	shader.roughness = 0.3f;
	shader.ao = 0.2f;
	shader.viewPos = camera.pos();
	shader.lightStrength = 10.0f;

	std::vector<Pipeline::FSIn<SimpleShader::VSToFS>> vertexOut = VertexProcessor::processVertex(vb, shader, { W_WIDTH, W_HEIGHT }, Primitive::TRIANGLE);

	if (renderMode != 2)
	{
		std::vector<Pipeline::FSIn<SimpleShader::VSToFS>> fragments = Rasterizer::rasterize(vertexOut);
		//std::cout << fragments.size() << std::endl;
		FragmentProcessor::processFragment(adapter, shader, fragments);
	}

	if (renderMode != 0) drawLines(vertexOut);

	flushScreen((BYTE*)colorBuffer.getCurrentBuffer().bufPtr(), W_WIDTH, W_HEIGHT);
	colorBuffer.swap();
	depthBuffer.swap();
}

int lastCursorX = W_WIDTH / 2;
int lastCursorY = W_HEIGHT / 2;
bool firstCursorMove = true;

void mouse(int x, int y, int button, int event)
{
	if (cursorDisabled)
	{
		std::cout << depthBuffer(x, y) << "  " << 1.0f / depthBuffer(x, y) << std::endl;
		return;
	}

    if (firstCursorMove)
    {
        lastCursorX = x;
        lastCursorY = y;
        firstCursorMove = false;
        return;
    }

    float offsetX = (x - lastCursorX) * CAMERA_ROTATE_SENSITIVITY;
    float offsetY = (y - lastCursorY) * CAMERA_ROTATE_SENSITIVITY;

    Vec3 offset = { -offsetX, -offsetY, 0.0f };
    camera.rotate(offset);

    lastCursorX = x;
    lastCursorY = y;
}

void keyboard(int key, int event)
{
	if (event == KEY_DOWN)
	{
		keyPressing[key] = true;
		if (key == VK_F1) cursorDisabled ^= 1;
		if (key == 'M') renderMode = (renderMode + 1) % 3;
	}
	if (event == KEY_UP) keyPressing[key] = false;
}

int Setup()
{
	initWindow("Test", DEFAULT, DEFAULT, W_WIDTH, W_HEIGHT);

	for (int i = 0; i < 36 * 8; i += 8)
	{
		SimpleShader::VSIn vertex;
		vertex.pos = { CUBE_VERTICES[i + 0], CUBE_VERTICES[i + 1], CUBE_VERTICES[i + 2] };
		vertex.texCoord = { CUBE_VERTICES[i + 3], CUBE_VERTICES[i + 4] };
		vertex.norm = { CUBE_VERTICES[i + 5], CUBE_VERTICES[i + 6], CUBE_VERTICES[i + 7] };
		vb.push_back(vertex);
	}
	
	camera.setFOV(75.0f);
	camera.setPlanes(0.1f, 100.0f);

	colorBuffer.fill({ 0, 0, 0 });
	depthBuffer.fill(1.0f);

	adapter.colorAttachments.push_back(&colorBuffer);
	adapter.depthAttachment = &depthBuffer;

	registerTimerEvent(render);
	registerMouseEvent(mouse);
	registerKeyboardEvent(keyboard);

	startTimer(0, 10);
}
