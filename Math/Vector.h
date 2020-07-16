#include <iostream>
#include <cstdlib>
#include <cmath>
#include <initializer_list>

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

	void operator += (Vec<N>& v)
	{
		for (int i = 0; i < N; i++) data[i] += v[i];
	}

	void operator -= (Vec<N>& v)
	{
		for (int i = 0; i < N; i++) data[i] -= v[i];
	}

	void operator *= (Vec<N>& v)
	{
		for (int i = 0; i < N; i++) data[i] *= v[i];
	}

	void operator /= (Vec<N>& v)
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

	Vec<N> operator + (Vec<N>& v)
	{
		Vec<N> res = *this;
		res += v;
		return res;
	}

	Vec<N> operator - (Vec<N>& v)
	{
		Vec<N> res = *this;
		res -= v;
		return res;
	}

	Vec<N> operator * (Vec<N>& v)
	{
		Vec<N> res = *this;
		res *= v;
		return res;
	}

	Vec<N> operator / (Vec<N>& v)
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

namespace VecKit
{
	template<int N>
	float dot(Vec<N>& a, Vec<N>& b)
	{
		float res = 0.0f;
		for (int i = 0; i < N; i++) res += a[i] * b[i];
		return res;
	}

	Vec<3> cross(Vec<3>&a, Vec<3>&b)
	{
		Vec<3> res;
		res[0] = a[1] * b[2] - a[2] * b[1];
		res[1] = a[2] * b[0] - a[0] * b[2];
		res[2] = a[0] * b[1] - a[1] * b[0];
		return res;
	}

	template<int N>
	float length(Vec<N> v)
	{
		float res = 0.0f;
		for (int i = 0; i < N; i++) res += v[i] * v[i];
		return sqrt(res);
	}
}
