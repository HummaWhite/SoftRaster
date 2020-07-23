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
			float z = adapter.getDepth(fragmentIn[i].x, fragmentIn[i].y);

			if (fragmentIn[i].z > z) continue;
			shader.processFragment(adapter, fragmentIn[i]);
			adapter.writeDepth(fragmentIn[i].x, fragmentIn[i].y, fragmentIn[i].z);
		}
	}

private:
};

#endif
