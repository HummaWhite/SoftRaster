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
		const int maxThread = 1;std::thread::hardware_concurrency();

		std::thread threads[maxThread];
		
		for (int i = 0; i < maxThread; i++)
		{
			int start = (fragmentIn.size() / maxThread) * i;
			int end = std::min(fragmentIn.size(), (fragmentIn.size() / maxThread) * (i + 1));

			threads[i] = std::thread
			(
				doProcess<Shader>,
				std::ref(adapter),
				std::ref(shader),
				std::ref(fragmentIn),
				start, end
			);	
		}

		for (auto& thread : threads)
		{
			thread.join();
		}
	}

private:
	template<typename Shader>
	static void doProcess(
			FrameBufferAdapter& adapter,
			Shader& shader,
			std::vector<Pipeline::FSIn<typename Shader::VSToFS>>& fragmentIn,
			int start,
			int end)
	{
		for (register int i = start; i < end; i++)
		{
			adapter.x = fragmentIn[i].x;
			adapter.y = fragmentIn[i].y;

			if (fragmentIn[i].z > adapter.readDepth()) continue;

			adapter.writeDepth(fragmentIn[i].z);
			shader.processFragment(adapter, fragmentIn[i]);
		}
	}
};

#endif
