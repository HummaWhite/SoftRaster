#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <cstdlib>

#include "stb_image/stb_image.h"
#include "math/Vector.h"
#include "Color.h"
#include "FrameBuffer.h"

typedef FrameBuffer<RGB24> TextureRGB24;
typedef FrameBuffer<RGBA32> TextureRGBA32;
typedef FrameBuffer<float> TextureFloat;

enum
{
	LINEAR = 0,
	NEAREST
} TextureFilterType;

namespace Texture
{
	BYTE* loadTexture(const char *filePath, int channels, int& width, int &height)
	{
		std::cout << "Loading Texture: " << filePath << std::endl;

		int bits;
		BYTE *data = stbi_load(filePath, &width, &height, &bits, channels);

		if (data == nullptr)
		{
			std::cout << "Error loading texture" << std::endl;
			exit(-1);
		}

		return data;
	}

	void load(TextureRGB24& tex, const char *filePath)
	{
		int width, height;
		BYTE *data = loadTexture(filePath, 3, width, height);

		tex.init(width, height);

		memcpy(tex.ptr(), data, width * height * sizeof(RGB24));
		stbi_image_free(data);
	}

	/*TextureRGBA32 loadRGBA32(const char *filePath)
	{
		TextureRGBA32 tex;
		int width, height;
		BYTE *data = loadTexture(filePath, 4, width, height);

		tex.init(width, height);

		memcpy(tex.ptr(), data, width * height * 4 * sizeof(BYTE));
		stbi_image_free(data);

		return tex;
	}*/
}

Vec4 texture(TextureRGB24& tex, Vec2 uv, int filterType)
{
	Vec4 res(1.0f);

	float x = (tex.width - 1) * uv[0];
	float y = (tex.height - 1) * uv[1];

	int lx = x, ly = y;

	if (filterType == NEAREST)
	{
		int u = (x - lx < lx + 1 - x) ? lx : lx + 1;
		int v = (y - ly < ly + 1 - y) ? ly : ly + 1;

		if (u < 0 || u >= tex.width || v < 0 || v >= tex.height) return Vec4(0.0f);

		std::cout << &tex << "  " << u << "  " << v << std::endl;

		RGB24 color = tex(u, v);

		return { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f };
	}

	return Vec4(1.0f);
}

Vec4 texture(TextureRGB24 *tex, Vec2 uv, int filterType)
{
	if (tex == nullptr) return Vec4(0.0f);

	Vec4 res(1.0f);

	float x = (tex->width - 1) * uv[0];
	float y = (tex->height - 1) * uv[1];

	int lx = x, ly = y;

	if (filterType == NEAREST)
	{
		int u = (x - lx < lx + 1 - x) ? lx : lx + 1;
		int v = (y - ly < ly + 1 - y) ? ly : ly + 1;
		
		if (u < 0 || u >= tex->width || v < 0 || v >= tex->height) return Vec4(0.0f);

		RGB24 color = (*tex)(u, v);

		return { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f };
	}

	return Vec4(1.0f);
}

#endif
