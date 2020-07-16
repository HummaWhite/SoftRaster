#include <iostream>
#include <cstdlib>
#include <cmath>
#include <initializer_list>

template<int N>
class Mat
{
public:
	Mat<N>()
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				data[i][j] = 0.0f;
	}

	Mat<N>(float v)
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				data[i][j] = (i == j) ? v : 0.0f;
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

	float& operator () (int i, int j)
	{
		return data[i][j];
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
