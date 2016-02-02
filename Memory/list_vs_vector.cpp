#include <chrono>
#include <list>
#include <vector>
#include <random>
#include <iostream>
#include<conio.h>

using namespace std;


class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<ms_>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::milli> ms_;
	std::chrono::time_point<clock_> beg_;
};



struct EpicStruct
{
	char m_memory[4];
};


// !!!!!!!!!!!!!!! This is a bad implementation of a vector. !!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!! No, really it is a bad one.               !!!!!!!!!!!!!!!!!!!!!!!
//Provided just as an example so people get a general idea of what's under the hood.
// Use this anywhere else besides 'demo' purposes and you deserve whatever bugs you get.
template<class T>
class MyVector
{
public:
	typedef T* iterator;

	MyVector()
	{
		m_AllocatedSize = 2;
		m_Size = 0;
		m_Data = new T[m_AllocatedSize];
	}

    ~MyVector()
    {
        delete[] m_Data;
    }

	void push_back(T element)
	{
		//One element is guaranteed to be available for end()
		// + 1 for the new one
		// + 1 for the end() reserved space.
		if ((m_Size + 2) >= m_AllocatedSize)
		{
			//Grow by half ( this is what microsoft SDL vector does. I know because I checked the code ).
			// Don't trust me for it, go check.
			size_t newSize = m_AllocatedSize + m_AllocatedSize / 2;
			T* m_NewData = new T[newSize];
			
			memcpy(m_NewData, m_Data, m_Size*sizeof(T));
			delete[] m_Data;
			
			m_AllocatedSize = newSize;
			m_Data = m_NewData;
		}
		m_Data[m_Size++] = element;
	}

	iterator begin()
	{
		return m_Data;
	}

	iterator end()
	{
		return m_Data + m_Size;
	}

	//Taken from here:
	// http://en.cppreference.com/w/cpp/algorithm/rotate
	void rotate(iterator first, iterator n_first, iterator last)
	{
		iterator next = n_first;
		while(first != n_first)
		{
			swap(*first, *next);

			first++;
			next++;

			if(next == last)
				next = n_first;
			else if (first == n_first)
				n_first = next;
		}
	}

	
//#define STL_WAY 
	void insert(iterator it, T value)
	{
#ifdef STL_WAY
		// This is how STL does it, using rotate, but not using temporary variables that hold ELEMENTS.
		// Uncomment the define above in order to enable this behaviour.
		size_t off = it - m_Data;
		push_back(value);
		rotate(m_Data + off, end() - 1, end());
#else   // MY way :)
		// Faster insert
		// Consumes more memory in theory due to a temporary variable that holds an extra ELEMENT - tmp
		size_t off = it - m_Data;
		push_back(value);
		iterator new_location = m_Data+off;
		T tmp = *(end() - 1);
		while(new_location != end())
		{
			std::swap(tmp, *new_location++);
		}
#endif
	}

	size_t size()
	{
		return m_Size;
	}

private:
	size_t m_Size;
	size_t m_AllocatedSize;
	T*     m_Data;
};


template<class T>
double test_container(size_t count)
{
	T l;
	T::iterator it;

	srand(42);
	Timer tmr;

	l.push_back(EpicStruct());

	for (size_t i = 0; i < count; ++i)
	{
		size_t pos = rand() % l.size();

		it = l.begin();
		for (size_t p = 0; p < pos; ++p)
			it++;

		l.insert(it, EpicStruct());
	}
	return tmr.elapsed();
}


int main()
{
	size_t count = 200000;

	double t = test_container<vector<EpicStruct>>(count);
	printf("Elapsed time vector: %.2f ms\n", t);

	t = test_container<list<EpicStruct>>(count);
	printf("Elapsed time list: %.2f ms\n", t);

	t = test_container<MyVector<EpicStruct>>(count);
	printf("Elapsed time MyVector: %.2f ms\n", t);

	_getch();
	return 0;
}
