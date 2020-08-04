#ifndef FRAMEBUFFERADAPTER_H
#define FRAMEBUFFERADAPTER_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "FrameBufferDouble.h"
#include "Color.h"

struct FrameBufferAdapter
{
	void writeColor(int index, Vec3 color)
	{
		if (index >= colorAttachments.size() || index < 0) return;
		if (colorAttachments[index] == nullptr) return;

		FrameBufferDouble<RGB24> *buf = colorAttachments[index];
		if (x < 0 || x >= buf->width() || y < 0 || y >= buf->height()) return;

		(*buf)(x, buf->height() - y - 1) = RGB24(color);
	}

	void writeDepth(float val)
	{
		if (depthAttachment == nullptr) return;

		FrameBufferDouble<float> *buf = depthAttachment;
		if (x < 0 || x >= buf->width() || y < 0 || y >= buf->height()) return;

		(*buf)(x, buf->height() - y - 1) = val;
	}

	float readDepth()
	{
		if (depthAttachment == nullptr) return 1.0f;

		FrameBufferDouble<float> *buf = depthAttachment;
		if (x < 0 || x >= buf->width() || y < 0 || y >= buf->height()) return 1.0f;

		return (*buf)(x, buf->height() - y - 1);
	}

	void swapBuffers()
	{
		for (auto buf : colorAttachments)
		{
			buf->swap();
		}

		if (depthAttachment)
		{
			depthAttachment->swap();
		}
	}

	std::vector<FrameBufferDouble<RGB24>*> colorAttachments;
	FrameBufferDouble<float> *depthAttachment = nullptr;
	int x, y;
};

#endif
