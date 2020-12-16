#ifndef APPLICATION_H
#define APPLICATION_H

#pragma GCC optimize(3, "Ofast", "inline")

#include <windows.h>
#include <iostream>
#include <olectl.h>
#include <iomanip>
#include <vector>
#include <memory>
#include <thread>

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
#include "FPSTimer.h"

class Application
{
public:
	~Application() {}

	void init(const std::string& name, HINSTANCE instance, int width, int height)
	{
		this->instance = instance;
		this->windowWidth = width;
		this->windowHeight = height;

		lastCursorX = width / 2;
		lastCursorY = height / 2;
		
		window = CreateWindowA(
			name.c_str(), name.c_str(),
			WS_OVERLAPPEDWINDOW & ~WS_MINIMIZEBOX & ~WS_SIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width, height,
			nullptr,
			nullptr,
			0,
			nullptr
		);

		SetWindowPos(window, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
		ShowWindow(window, 1);
		UpdateWindow(window);

		colorBuffer.init(width, height);
		initRenderData();
	}

	LRESULT process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			auto metricX = GetSystemMetrics(SM_CXSCREEN);
			auto metricY = GetSystemMetrics(SM_CYSCREEN);

			HDC hdc = GetDC(hWnd);
			bitmap = CreateCompatibleBitmap(hdc, metricX, metricY);

			memdc = CreateCompatibleDC(hdc);
			SelectObject(memdc, bitmap);
			
			BitBlt(memdc, 0, 0, metricX, metricY, memdc, 0, 0, WHITENESS);

			DeleteDC(memdc);
			ReleaseDC(hWnd, hdc);

			break;
		}

		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			RECT rect;

			hdc = BeginPaint(hWnd, &ps);
			memdc = CreateCompatibleDC(hdc);
			SelectObject(memdc, bitmap);
			GetClientRect(hWnd, &rect);

			BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);

			DeleteDC(memdc);
			memdc = 0;
			EndPaint(hWnd, &ps);

			break;
		}

		case WM_KEYDOWN:
		{
			keyPressing[(int)wParam] = true;
			if ((int)wParam == 'M') renderer.renderMode = (renderer.renderMode + 1) % 4;
			break;
		}

		case WM_KEYUP:
			keyPressing[(int)wParam] = false;
			break;

		case WM_MOUSEMOVE:
		{
			if (cursorDisabled) break;

			if (firstCursorMove)
			{
				lastCursorX = (int)LOWORD(lParam);
				lastCursorY = (int)HIWORD(lParam);
				firstCursorMove = false;
			}

			float offsetX = ((int)LOWORD(lParam) - lastCursorX) * CAMERA_ROTATE_SENSITIVITY;
    		float offsetY = ((int)HIWORD(lParam) - lastCursorY) * CAMERA_ROTATE_SENSITIVITY;

    		Vec3 offset({ -offsetX, -offsetY, 0.0f });
    		camera.rotate(offset);

   			lastCursorX = (int)LOWORD(lParam);
    		lastCursorY = (int)HIWORD(lParam);
			break;
		}

		case WM_DESTROY:
			DeleteObject(bitmap);
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void render()
	{
		processKey();
		colorBuffer.fill({ 0, 0, 0 });;
		depthBuffer.fill(1.0f);
		fpsTimer.work();

		if (!cursorDisabled) processKey();

		//model = rotate(model, { 0.0f, 1.0f, 0.0f }, 1.0f);
		Mat3 m(model);
		m = inverse(m).transpose();

		shader.model = model;
		shader.view = camera.viewMatrix();
		shader.proj = camera.projMatrix(windowWidth, windowHeight);
		shader.albedo = { 1.0f, 0.6f, 0.4f };
		shader.metallic = 0.0f;
		shader.roughness = 1.0f;
		shader.ao = 0.1f;
		shader.viewPos = camera.pos();
		shader.lightStrength = exp(3.0f);
		shader.tex = &tex;
		shader.env = &env;
		shader.lightPos = lightPos;
		shader.lightColor = lightColor;

		renderer.draw(vb, shader, adapter);

		flushScreen();
		adapter.swapBuffers();
	}

private:
	void initRenderData()
	{
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
		Texture::load(env, "texture/pixel.png");

		renderer.cullFaceMode = CULL_BACK;

		depthBuffer.init(windowWidth, windowHeight);
		colorBuffer.init(windowWidth, windowHeight);
	}

	void flushScreen()
	{
		BITMAPINFO bInfo;
		ZeroMemory(&bInfo, sizeof(BITMAPINFO));

		auto &header = bInfo.bmiHeader;
		header.biBitCount		= 24;
		header.biCompression	= BI_RGB;
		header.biWidth			= windowWidth;
		header.biHeight			= -windowHeight;
		header.biPlanes			= 1;
		header.biSizeImage		= 0;
		header.biSize			= sizeof(BITMAPINFOHEADER);

		HDC dc = GetDC(window);
		HDC compatibleDC = CreateCompatibleDC(dc);
		HBITMAP compatibleBitmap = CreateCompatibleBitmap(dc, windowWidth, windowHeight);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(compatibleDC, compatibleBitmap);

		SetDIBits
		(
			dc,
			compatibleBitmap,
			0,
			windowHeight,
			(BYTE*)colorBuffer.getCurrentBuffer().bufPtr(),
			&bInfo,
			DIB_RGB_COLORS
		);
		BitBlt(dc, -1, -1, windowWidth, windowHeight, compatibleDC, 0, 0, SRCCOPY);

		DeleteDC(compatibleDC);
		DeleteObject(oldBitmap);
		DeleteObject(compatibleBitmap);
		UpdateWindow(window);
	}

	void processKey()
	{
		if (keyPressing['W']) camera.move('W');
		if (keyPressing['S']) camera.move('S');
		if (keyPressing['A']) camera.move('A');
		if (keyPressing['D']) camera.move('D');
		if (keyPressing[VK_SHIFT]) camera.move(VK_SHIFT);
		if (keyPressing[VK_SPACE]) camera.move(VK_SPACE);
	}

private:
	std::string name;
	int windowWidth;
	int windowHeight;

	HINSTANCE instance;
	HWND window;
	HDC memdc;
	HBITMAP bitmap;

	bool keyPressing[256];
	bool F1Pressed = false;
	bool cursorDisabled = true;
	int lastCursorX;
	int lastCursorY;
	bool firstCursorMove = true;


	FrameBufferDouble<float> depthBuffer;
	FrameBufferDouble<RGB24> colorBuffer;
	SimpleShader shader;
	Camera camera = Camera({ 0.0f, -5.0f, 3.0f });
	FrameBufferAdapter adapter;
	Renderer renderer;
	TextureRGB24 tex;
	TextureRGB24 env;
	Vec3 lightPos = { 1.0f, -2.0f, 3.0f };
	Vec3 lightColor = { 1.0f, 1.0f, 1.0f };

	Mat4 model = Mat4(1.0f);
	std::vector<SimpleShader::VSIn> vb;

	FPSTimer fpsTimer;
};

#endif
