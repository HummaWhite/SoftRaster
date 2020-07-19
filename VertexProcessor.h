#ifndef VERTEXPROCESSOR_H
#define VERTEXPROCESSOR_H

#include <vector>

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Buffer.h"
#include "Shader.h"
#include "Primitive.h"

class VertexProcessor
{
public:
	template<typename Shader>
	static void processVertex(
			Buffer<typename Shader::OutType>& vertexOut,
			Buffer<typename Shader::InType>& vertexIn,
			Shader& shader,
			int primitiveType,
			Buffer<UINT> *indices = nullptr)
	{
		int vertexCount = (indices == nullptr) ? vertexIn.count : indices->count;
		Buffer<typename Shader::OutType> clipSpaceVertexData(vertexCount);

		for (int i = 0; i < vertexCount; i++)
		{
			UINT index = (indices == nullptr) ? i : (*indices)[i];
			
			typename Shader::OutType tmp = shader.process(vertexIn[index]);
			clipSpaceVertexData[i] = tmp;
		}

		// TODO:
		// for each primitive p in clipSpaceVertexData:
		// clip(p) -> to ndc -> add to finalBuffer
		// finalBuffer -> vertexOut

		switch (primitiveType)
		{
			case Primitive::LINE:
				break;
			case Primitive::TRIANGLE:
				clipTriangle(vertexOut, clipSpaceVertexData);
				break;
			case Primitive::POINT:
			default:
				break;
		};

		for (int i = 0; i < vertexOut.count; i++)
		{
			vertexOut[i].sr_Position /= vertexOut[i].sr_Position[3];
		}
	}

private:
	template<typename ShaderOutType>
	static void clipTriangle(
			Buffer<ShaderOutType>& outData,
			Buffer<ShaderOutType>& clipSpaceVertexData)
	{
		outData.copy(clipSpaceVertexData);
	}
};

#endif
