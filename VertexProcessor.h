#ifndef VERTEXPROCESSOR_H
#define VERTEXPROCESSOR_H

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"

template<typename VertexOut, typename VertexIn, typename VertexShader>
class VertexProcessor
{
	static Buffer<VertexOut>& processVertex(
			Buffer<VertexIn>& vertexIn,
			VertexShader& shader,
			Buffer<UINT> *index = nullptr)
	{
		// TODO:
		// for each vertex v in vertexIn:
		// shader.process(v) -> add to tmpBuffer
		// for each primitive p in tmpBuffer:
		// clip(p) -> add to finalBuffer
		// finalBuffer -> vertexOut
		shader.process<VertexOut, VertexIn>...
	}
};

#endif
