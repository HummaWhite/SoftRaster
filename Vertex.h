#ifndef VERTEX_H
#define VERTEX_H

#include "math/Vector.h"

namespace Vertex
{
	struct POS3_UV2
	{
		Vec3 pos;
		Vec2 uv;
	};

	struct POS3_UV2_NORM3
	{
		Vec3 pos;
		Vec2 uv;
		Vec3 norm;
	};
}

#endif
