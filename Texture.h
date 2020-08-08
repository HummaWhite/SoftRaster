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

inline Vec4 texture(TextureRGB24* tex, Vec2 uv, int filterType)
{
	if (tex == nullptr) return Vec4(0.0f);

	Vec4 res(1.0f);

	float x = (tex->width - 1) * uv[0];
	float y = (tex->height - 1) * uv[1];

	if (filterType == NEAREST)
	{
		int u = (int)(x - 0.5f + tex->width) % tex->width;
		int v = (int)(y - 0.5f + tex->height) % tex->height;

		return (*tex)(u, v).toVec4();
	}
	else if (filterType == LINEAR)
	{
		int u1 = (int)(x + tex->width) % tex->width;
		int v1 = (int)(y + tex->height) % tex->height;
		int u2 = (int)(x + 1.0f + tex->width) % tex->width;
		int v2 = (int)(y + 1.0f + tex->height) % tex->height;

		Vec4 c1 = (*tex)(u1, v1).toVec4();
		Vec4 c2 = (*tex)(u2, v1).toVec4();
		Vec4 c3 = (*tex)(u1, v2).toVec4();
		Vec4 c4 = (*tex)(u2, v2).toVec4();

		float lx = x - (int)x;
		float ly = y - (int)y;

		return lerp(lerp(c1, c2, lx), lerp(c3, c4, lx), ly);
	}
}

#endif
