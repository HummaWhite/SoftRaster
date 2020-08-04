#ifndef OBJREADER_H
#define OBJREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdio>

#include "Buffer.h"
#include "math/Vector.h"

struct ObjReader
{
	std::vector<float> readFile(const char* filePath)
	{
		std::vector<float> data;
		std::fstream file(filePath);
		std::cout << "Loading Obj: " << filePath << std::endl;

		if (!file.is_open())
		{
			std::cout << "Error loading Obj" << std::endl;
			return data;
		}

		std::vector<Vec3> points;
		std::vector<Vec2> texCoords;
		std::vector<Vec3> normals;

		std::string line;
		while (std::getline(file, line))
		{
			if (line.empty()) continue;
			if (line[0] != 'v' && line[0] != 'f') continue;

			std::stringstream ss;
			ss << line;

			std::string type;
			ss >> type;

			if (type == "f")
			{
				int indexP[3], indexT[3], indexN[3];

				const char *buf = line.c_str();
				sscanf
				(
					buf + 1,
					"%d/%d/%d %d/%d/%d %d/%d/%d",
					&indexP[0], &indexT[0], &indexN[0],
					&indexP[1], &indexT[1], &indexN[1],
					&indexP[2], &indexT[2], &indexN[2]
				);

				for (int i = 0; i < 3; i++)
				{
					Vec3 p = points[indexP[i] - 1];
					Vec2 t = texCoords[indexT[i] - 1];
					Vec3 n = normals[indexT[i] - 1];

					for (int j = 0; j < 3; j++) data.push_back(p[j]);
					for (int j = 0; j < 2; j++) data.push_back(t[j]);
					for (int j = 0; j < 3; j++) data.push_back(n[j]);
				}
			}
			else if (type == "v")
			{
				Vec3 v;
				ss >> v[0] >> v[1] >> v[2];
				points.push_back(v);
			}
			else if (type == "vt")
			{
				Vec2 v;
				ss >> v[0] >> v[1];
				texCoords.push_back(v);
			}
			else if (type == "vn")
			{
				Vec3 v;
				ss >> v[0] >> v[1] >> v[2];
				normals.push_back(v);
			}
		}

		return data;
	}
};

#endif
