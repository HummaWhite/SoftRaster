#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <initializer_list>
#include "Math.h"

template<int N>
class Vec
{
public:
	Vec<N>()
	{
		for (int i = 0; i < N; i++) data[i] = 0.0f;
	}

	template<int M>
	Vec<N>(Vec<M>& vec)
	{
		int limit = N > M ? M : N;
		for (int i = 0; i < limit; i++) data[i] = vec[i];
		if (N > M)
		{
			for (int i = M; i <= N; i++) data[i] = 0.0f;
		}
	}

	Vec<N>(float v)
	{
		for (int i = 0; i < N; i++) data[i] = v;
	}

	Vec<N>(std::initializer_list<float> list)
	{
		if (list.size() != N)
		{
			std::cout << "Vector::Error: bad initialization" << std::endl;
			exit(-1);
		}

		int i = 0;
		for (auto v : list)
		{
			data[i++] = v;
		}
	}

	float& operator [] (int index)
	{
		if (index < 0 || index >= N)
		{
			std::cout << "Vector::Error: index out of bound" << std::endl;
			exit(-1);
		}
		return data[index];
	}

	Vec<N> operator - ()
	{
		Vec<N> res;
		for (int i = 0; i < N; i++) res[i] = -data[i];
		return res;
	}

	void operator += (Vec<N> v)
	{
		for (int i = 0; i < N; i++) data[i] += v[i];
	}

	void operator -= (Vec<N> v)
	{
		for (int i = 0; i < N; i++) data[i] -= v[i];
	}

	void operator *= (Vec<N> v)
	{
		for (int i = 0; i < N; i++) data[i] *= v[i];
	}

	void operator /= (Vec<N> v)
	{
		for (int i = 0; i < N; i++) data[i] /= v[i];
	}

	void operator *= (float v)
	{
		for (int i = 0; i < N; i++) data[i] *= v;
	}

	void operator /= (float v)
	{
		for (int i = 0; i < N; i++) data[i] /= v;
	}

	Vec<N> operator + (Vec<N> v)
	{
		Vec<N> res = *this;
		res += v;
		return res;
	}

	Vec<N> operator - (Vec<N> v)
	{
		Vec<N> res = *this;
		res -= v;
		return res;
	}

	Vec<N> operator * (Vec<N> v)
	{
		Vec<N> res = *this;
		res *= v;
		return res;
	}

	Vec<N> operator / (Vec<N> v)
	{
		Vec<N> res = *this;
		res /= v;
		return res;
	}

	Vec<N> operator * (float v)
	{
		Vec<N> res = *this;
		res *= v;
		return res;
	}

	Vec<N> operator / (float v)
	{
		Vec<N> res = *this;
		res /= v;
		return res;
	}

	float length()
	{
		float res = 0.0f;
		for (int i = 0; i < N; i++) res += data[i] * data[i];
		return sqrt(res);
	}

	Vec<N> normalized()
	{
		return (*this) / length();
	}

	void* ptr()
	{
		return (void*)data;
	}

	friend std::ostream& operator << (std::ostream& out, Vec<N>& v)
	{
		for (int i = 0; i < N; i++)
		{
			out << std::fixed << v.data[i] << " ";
		}
	}

	void print()
	{
		std::cout << *this << std::endl;
	}

private:
	float data[N];
};

typedef Vec<4> Vec4;
typedef Vec<3> Vec3;
typedef Vec<2> Vec2;

template<int N>
float dot(Vec<N> a, Vec<N> b)
{
	float res = 0.0f;
	for (int i = 0; i < N; i++) res += a[i] * b[i];
	return res;
}

inline static Vec3 cross(Vec3 a, Vec3 b)
{
	Vec3 res;
	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
	return res;
}

inline static float cross(Vec2 a, Vec2 b)
{
	return (a[0] * b[1] - a[1] * b[0]) * 0.5f;
}

template<int N>
float length(Vec<N> v)
{
	return v.length();
}

template<int N>
Vec<N> normalize(Vec<N> v)
{
	return v.normalized();
}

template<int N>
Vec<N> pow(Vec<N> v, float x)
{
	Vec<N> res;
	for (int i = 0; i < N; i++) res[i] = pow(v[i], x);
	return res;
}

template<int N>
Vec<N> pow(Vec<N> v, Vec<N> x)
{
	Vec<N> res;
	for (int i = 0; i < N; i++) res[i] = pow(v[i], x[i]);
	return res;
}

template<typename T>
T lerp(T from, T to, float weight)
{
	return from + (to - from) * weight;
}

template<typename T>
T triLerp(T a, T b, T c, Vec3 weight)
{
	return a * weight[0] + b * weight[1] + c * weight[2];
}

template<int N>
bool equals(Vec<N>& a, Vec<N>& b, float eps)
{
	bool equal = true;
	for (int i = 0; i < N; i++)
	{
		if (abs(a[i] - b[i]) > eps) equal = false;
	}
	return equal;
}

#endif
