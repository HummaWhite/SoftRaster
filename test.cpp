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
#include "Renderer.h"
#include "ObjReader.h"
#include "Texture.h"

const int W_WIDTH = 960;
const int W_HEIGHT = 540;

bool keyPressing[256] = { false };

FrameBufferDouble<float> depthBuffer(W_WIDTH, W_HEIGHT);
FrameBufferDouble<RGB24> colorBuffer(W_WIDTH, W_HEIGHT);
Mat4 model(1.0f);
SimpleShader shader;
Camera camera({ 0.0f, -5.0f, 2.0f });
FrameBufferAdapter adapter;
Renderer renderer;
TextureRGB24 tex;

std::vector<SimpleShader::VSIn> vb;

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

	//model = rotate(model, { 0.0f, 1.0f, 0.0f }, 1.0f);
	Mat3 m(model);
	m = inverse(m).transpose();

	shader.model = model;
	shader.view = camera.viewMatrix();
	shader.proj = camera.projMatrix(W_WIDTH, W_HEIGHT);
	shader.albedo = { 0.5f, 0.7f, 0.9f };
	shader.metallic = 1.0f;
	shader.roughness = 0.6f;
	shader.ao = 0.2f;
	shader.viewPos = camera.pos();
	shader.lightStrength = 20.0f;
	shader.tex = &tex;

	renderer.draw(vb, shader, adapter);

	flushScreen((BYTE*)colorBuffer.getCurrentBuffer().bufPtr(), W_WIDTH, W_HEIGHT);
	adapter.swapBuffers();
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
		if (key == 'M') renderer.renderMode = (renderer.renderMode + 1) % 4;
	}
	if (event == KEY_UP) keyPressing[key] = false;
}

int Setup()
{
	initWindow("Test", DEFAULT, DEFAULT, W_WIDTH, W_HEIGHT);

	ObjReader objReader;
	std::vector<float> data = objReader.readFile("model/teapot20.obj");
	std::cout << data.size() << "\n";

	for (int i = 0; i < data.size(); i += 8)
	{
		SimpleShader::VSIn vertex;
		vertex.pos = { data[i + 0], data[i + 1], data[i + 2] };
		vertex.texCoord = { data[i + 3], data[i + 4] };
		vertex.norm = { data[i + 5], data[i + 6], data[i + 7] };
		vb.push_back(vertex);
	}

	model = rotate(model, { 1.0f, 0.0f, 0.0f }, 90.0f);
	
	camera.setFOV(75.0f);
	camera.setPlanes(0.1f, 100.0f);

	colorBuffer.fill({ 0, 0, 0 });
	depthBuffer.fill(1.0f);

	adapter.colorAttachments.push_back(&colorBuffer);
	adapter.depthAttachment = &depthBuffer;

	Texture::load(tex, "texture/diamond_ore.png");

	registerTimerEvent(render);
	registerMouseEvent(mouse);
	registerKeyboardEvent(keyboard);

	startTimer(0, 10);
}
