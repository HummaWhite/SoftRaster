#ifndef FRAGMENTPROCESSOR_H
#define FRAGMENTPROCESSOR_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "FrameBufferAdapter.h"
#include "Shader.h"

class FragmentProcessor
{
public:
	template<typename Shader>
	static void processFragment(
			FrameBufferAdapter& adapter,
			Shader& shader,
			std::vector<Pipeline::FSIn<typename Shader::VSToFS>>& fragmentIn)
	{
		for (int i = 0; i < fragmentIn.size(); i++)
		{
			adapter.x = fragmentIn[i].x;
			adapter.y = fragmentIn[i].y;

			if (fragmentIn[i].z > adapter.readDepth()) continue;

			adapter.writeDepth(fragmentIn[i].z);
			shader.processFragment(adapter, fragmentIn[i]);
		}
	}

private:
};

#endif
