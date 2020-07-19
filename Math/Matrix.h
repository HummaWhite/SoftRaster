#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <initializer_list>
#include "Vector.h"
#include "Math.h"

template<int N>
class Mat
{
public:
	Mat<N>()
	{
		memset(data, 0, sizeof data);
	}

	Mat<N>(float v)
	{
		*this = Mat<N>();
		for (int i = 0; i < N; i++) data[i][i] = v;
	}

	Mat<N>(std::initializer_list<float> list)
	{
		if (list.size() != N)
		{
			std::cout << "Matrix::Error: bad initialization" << std::endl;
			exit(-1);
		}

		*this = Mat<N>();

		int i = 0;
		for (auto v : list)
		{
			data[i][i] = v;
			i++;
		}
	}

	Mat<N>(std::initializer_list<Vec<N>> list)
	{
		if (list.size() != N)
		{
			std::cout << "Matrix::Error: bad initialization" << std::endl;
			exit(-1);
		}

		int i = 0;
		for (auto v : list)
		{
			for (int j = 0; j < N; j++)
			{
				data[i][j] = v[j];
			}
			i++;
		}
	}

	float& operator () (int i, int j)
	{
		return data[i][j];
	}

	Mat<N> operator - ()
	{
		return (*this) * -1.0f;
	}

	Mat<N> operator + (Mat<N>& m)
	{
		Mat<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res(i, j) = data[i][j] + m(i, j);
			}
		}
		return res;
	}

	Mat<N> operator - (Mat<N>& m)
	{
		Mat<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res(i, j) = data[i][j] - m(i, j);
			}
		}
		return res;
	}

	Mat<N> operator * (Mat<N>& m)
	{
		Mat<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				for (int k = 0; k < N; k++)
				{
					res(i, j) += data[i][k] * m(k, j);
				}
			}
		}
		return res;
	}

	Mat<N> operator * (float v)
	{
		Mat<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res(i, j) = data[i][j] * v;
			}
		}
		return res;
	}

	Mat<N> operator / (float v)
	{
		Mat<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res(i, j) = data[i][j] / v;
			}
		}
		return res;
	}

	Vec<N> operator * (Vec<N>& v)
	{
		Vec<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res[i] += data[i][j] * v[j];
			}
		}
		return res;
	}

	Mat<N> transpose()
	{
		Mat<N> res;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res(i, j) = data[j][i];
			}
		}
		return res;
	}

	friend std::ostream& operator << (std::ostream& out, Mat<N>& m)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				out << std::fixed << m.data[i][j] << " ";
			}
			out << std::endl;
		}
	}

	void print()
	{
		std::cout << *this << std::endl;
	}

private:
	float data[N][N];
};

typedef Mat<4> Mat4;
typedef Mat<3> Mat3;

template<int N>
Mat<N> transpose(Mat<N>& m)
{
	return m.transpose();
}

Mat4 translationMatrix(Vec3 delta)
{
	Mat4 res(1.0f);
	for (int i = 0; i < 3; i++) res(i, 3) = delta[i];
	return res;
}

Mat4 scaleMatrix(Vec3 scale)
{
	return { scale[0], scale[1], scale[2], 1.0f };
}

Mat4 rotationMatrix(Vec3 axis, float deg)
{
	float t = toRad(deg);
	float sint = sin(t);
	float cost = cos(t);
	float x = axis[0], y = axis[1], z = axis[2];
	return
	{
		{ x * x * (1 - cost) + 1 * cost, x * y * (1 - cost) - z * sint, x * z * (1 - cost) + y * sint, 0.0f },
		{ x * y * (1 - cost) + z * sint, y * y * (1 - cost) + 1 * cost, y * z * (1 - cost) - x * sint, 0.0f },
		{ x * z * (1 - cost) - y * sint, y * z * (1 - cost) + x * sint, z * z * (1 - cost) + 1 * cost, 0.0f },
		{                          0.0f,                          0.0f,                          0.0f, 1.0f }
	};

}

Mat4 translate(Mat4& m, Vec3 delta)
{
	Mat4 tr = translationMatrix(delta);
	return m * tr;
}

Mat4 scale(Mat4& m, Vec3 scale)
{
	Mat4 sc = scaleMatrix(scale);
	return m * sc;
}

Mat4 rotate(Mat4& m, Vec3 axis, float deg)
{
	Mat4 rt = rotationMatrix(axis.normalized(), deg);
	return m * rt;
}

float det(Mat3& m)
{
	float res
		= m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
		- m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
		+ m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
	return res;
}

Mat3 inverse(Mat3& m)
{
	float d = det(m);
	Mat3 res =
	{
		{ m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1), m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2), m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1) },
		{ m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2), m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0), m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2) },
		{ m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0), m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1), m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0) }
	};
	return res / d;
}

Mat4 inverse(Mat4& m)
{
	float d
		= m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
		- m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
		+ m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));

	Mat4 res;

	//左上角3x3矩阵的逆
	Mat3 ltInv =
	{
		{ m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1), m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2), m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1) },
		{ m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2), m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0), m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2) },
		{ m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0), m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1), m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0) }
	};
	ltInv = ltInv / d;

	//右上角1x3矩阵
	Vec3 rt = { m(0, 3), m(1, 3), m(2, 3) };
	rt = (-ltInv) * rt;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			res(i, j) = ltInv(i, j);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		res(i, 3) = rt[i];
	}

	res(3, 3) = 1.0f;
	return res;
}

Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Mat4 res;

	res(0, 0) = 2.0f / (right - left);
	res(1, 1) = 2.0f / (top - bottom);
	res(2, 2) = 2.0f / (zNear - zFar);
	res(3, 3) = 1.0f;
	res(0, 3) = (left + right) / (left - right);
	res(1, 3) = (bottom + top) / (bottom - top);
	res(2, 3) = (zNear + zFar) / (zNear - zFar);

	return res;
}

Mat4 perspective(float FOVy, float aspect, float zNear, float zFar)
{
	Mat4 res;

	float tant = tan(toRad(FOVy) / 2.0f);
	res(0, 0) = 1.0f / aspect / tant;
	res(1, 1) = 1.0f / tant;
	res(2, 2) = (zNear + zFar) / (zNear - zFar);
	res(2, 3) = 2 * zNear * zFar / (zNear - zFar);
	res(3, 2) = -1.0f;

	return res;
}

Mat4 lookAt(Vec3 eye, Vec3 lookingAt, Vec3 up)
{
	Vec3 D = (lookingAt - eye).normalized();
	Vec3 R = cross(D, up).normalized();
	Vec3 U = cross(R, D).normalized();
	return
	{
		{ R[0], R[1], R[2], -dot(R, eye) },
		{ U[0], U[1], U[2], -dot(U, eye) },
		{-D[0],-D[1],-D[2], +dot(D, eye) },
		{ 0.0f, 0.0f, 0.0f,         1.0f }
	};
}

#endif
