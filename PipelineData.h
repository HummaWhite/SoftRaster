#ifndef PIPELINEDATA_H
#define PIPELINEDATA_H

#include <vector>

#include "math/Math.h"
#include "math/Vector.h"
#include "math/Matrix.h"

namespace Pipeline
{
	template<typename VSToFS>
	struct VSOut
	{
		VSOut(): sr_Position(0.0f) {}
		VSOut(VSOut& a, VSOut& b, float weight)
		{
			data = VSToFS(a.data, b.data, weight);
			sr_Position = lerp(a.sr_Position, b.sr_Position, weight);
		}

		VSToFS data;
		Vec4 sr_Position;
	};

	template<typename VSToFS>
	struct FSIn
	{
		FSIn(): x(0), y(0), z(0.0f) {}
		FSIn(FSIn& a, FSIn& b, FSIn& c, Vec3 weight)
		{
			z = dot(weight, Vec3{ a.z, b.z, c.z });
			Vec3 correctedWeight = weight * Vec3{ a.z, b.z, c.z } / z;

			data = VSToFS(a.data, b.data, c.data, correctedWeight);
		}

		VSToFS data;
		int x, y;
		float z;
	};
}

#endif
