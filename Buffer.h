#include <Windows.h>
#include <cstdlib>
#include <algorithm>

template<typename T>
struct Buffer
{
	Buffer() {}

	Buffer(int size)
	{
		init(size);
	}

	~Buffer()
	{
		release();
	}

	virtual void init(int size)
	{
		if (data != nullptr) return;
		data = new T[size];
		this->size = size;
	}

	virtual void release()
	{
		if (data == nullptr) return;
		delete[] data;
		data = nullptr;
		size = 0;
	}

	virtual void resize(int size)
	{
		release();
		init(size);
	}

	void fill(T val)
	{
		std::fill(data, data + size, val);
	}

	T& operator () (int index)
	{
		return data[index];
	}

	void* bufPtr()
	{
		return (void*)data;
	}

	T* ptr()
	{
		return data;
	}

	T *data = nullptr;
	int size = 0;
};
