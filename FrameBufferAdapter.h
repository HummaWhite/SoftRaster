#ifndef FRAMEBUFFERADAPTER_H
#define FRAMEBUFFERADAPTER_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "FrameBufferDouble.h"
#include "Color.h"

struct FrameBufferAdapter
{
	void writeColor(int index, int x, int y, Vec3 color)
	{
		if (index >= colorAttachments.size()) return;
		if (colorAttachments[index] == nullptr) return;

		FrameBufferDouble<RGB24> *buf = colorAttachments[index];
		if (x < 0 || x >= buf->width() || y < 0 || y >= buf->height()) return;

		(*buf)(x, buf->height() - y) = RGB24(color);
	}

	void writeDepth(int x, int y, float val)
	{
		if (depthAttachment == nullptr) return;

		FrameBufferDouble<float> *buf = depthAttachment;
		if (x < 0 || x >= buf->width() || y < 0 || y >= buf->height()) return;

		(*buf)(x, buf->height() - y) = val;
	}

	float getDepth(int x, int y)
	{
		if (depthAttachment == nullptr) return 1.0f;

		FrameBufferDouble<float> *buf = depthAttachment;
		if (x < 0 || x >= buf->width() || y < 0 || y >= buf->height()) return 1.0f;

		return (*buf)(x, buf->height() - y);
	}

	std::vector<FrameBufferDouble<RGB24>*> colorAttachments;
	FrameBufferDouble<float> *depthAttachment;
};

#endif
