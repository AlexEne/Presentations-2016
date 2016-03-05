#include <chrono>
#include <list>
#include <vector>
#include <random>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
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
#define EpicStruct_SIZE 4
	char m_memory[EpicStruct_SIZE];

	EpicStruct()
	{
	}

	explicit EpicStruct(size_t val)
	{
		memset(m_memory, val % 127, sizeof(m_memory));
	}

	void print()
	{
		for (int i = 0; i < EpicStruct_SIZE; ++i)
			printf("%d", m_memory[i]);
	}
};

struct ListElement
{
	ListElement * next;
	ListElement * prev;
	EpicStruct  value;
};

template <class T>
class MemoryManagerArray
{
	const unsigned int c_maxNumElements = 0x20000;
	const unsigned int c_blockSize = sizeof(T) * c_maxNumElements;

	//pointer to beginging of allocated memory
	T	*m_allocatedMemory;
	unsigned int    m_currentIdx;
public:

	MemoryManagerArray():m_currentIdx(0)
	{
		m_allocatedMemory = (T*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, c_blockSize);
	}

	T* getNewElement()
	{
		if (m_currentIdx < c_maxNumElements)
		{
			return &m_allocatedMemory[m_currentIdx++];
		}
		else
		{
			assert(0);
			return NULL;
		}
	}

	void deleteElement(T * el) const
	{
		//NOTHING
	}

	~MemoryManagerArray()
	{
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, m_allocatedMemory);
	}

};

// NO NON-POD Classes supported (because of offsetof)
template <class T>
class MemoryManagerList
{
	const unsigned int c_maxNumeElements = 0x20000;
	const unsigned int c_blockSize = sizeof(MemElement) * c_maxNumeElements;
	
	//pointer to beginging of allocated memory
	unsigned char	*m_allocatedMemory;
	//value field offset in structure
	unsigned int	m_structValueOffset;
	//List of all memory blocks 
	struct MemElement
	{
		MemElement *next;
		T val;
	} * m_freeList;

public:

	MemoryManagerList()
	{
		m_allocatedMemory = (unsigned char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, c_blockSize);

		m_freeList = (MemElement*)m_allocatedMemory;
		MemElement * listIter = m_freeList;
		// link the free list elements, they are first held in contigous memory
		for (unsigned int i = 0; i < c_maxNumeElements; i++)
		{
			listIter->next = &(m_freeList[i]);
			listIter = listIter->next;
		}
		// mark the end of the list
		listIter->next = NULL;
		// hold the value field adress offset
		m_structValueOffset = (size_t)offsetof(MemElement, val);
	}

	T * getNewElement()
	{
		if (m_freeList != NULL)
		{
			// remove the element from the free list
			MemElement * newElement = m_freeList;
			m_freeList = m_freeList->next;
			return (&newElement->val);
		}
		else
		{
			assert(0);
			return NULL;
		}
	}

	void deleteElement(ListElement * el)
	{
		MemElement * melAddr = (MemElement*)((size_t*)el - m_structValueOffset);
		melAddr->next = m_freeList;
		m_freeList = melAddr;
	}

	~MemoryManagerList()
	{
		HeapFree(GetProcessHeap(),HEAP_ZERO_MEMORY, m_allocatedMemory);
	}

};

MemoryManagerArray<ListElement> g_mmArray;
MemoryManagerList<ListElement> g_mmListDelete;

double test_containerCustomArray(size_t count)
{
	srand(42);
	Timer tmr;

	ListElement * root = g_mmArray.getNewElement();
	root->next = root->prev = NULL;
	ListElement * prev = root;
	size_t currentSize = 1;
	// generate big list
	for (unsigned int i = 0; i < count; i++)
	{
		size_t pos = rand() % currentSize;
		ListElement *newel = g_mmArray.getNewElement();
		ListElement * p = root;
		for (unsigned int j = 0; j < pos; j++)
		{
			p = p->next;
		}
		ListElement* l = p->next;
		newel->next = l;
		newel->prev = p;
		p->next = newel;
		if (l != NULL)
			l->prev = newel;
		currentSize++;
	}
	double t = tmr.elapsed();

	Timer tmrdelete;
	ListElement * p = root;
	while (p != NULL)
	{
		ListElement* del = p;
		p = p->next;
		g_mmArray.deleteElement(del);
	}
	printf("tmrdelete : %f \n", tmrdelete.elapsed());

	return t;
}


double test_containerCustomList(size_t count)
{
	srand(42);
	Timer tmr;

	ListElement * root = g_mmListDelete.getNewElement();
	root->next = root->prev = NULL;
	ListElement * prev = root;
	size_t currentSize = 1;
	// generate big list
	for (unsigned int i = 0; i < count; i++)
	{
		size_t pos = rand() % currentSize;
		ListElement *newel = g_mmListDelete.getNewElement();
		ListElement * p = root;
		for (unsigned int j = 0; j < pos; j++)
		{
			p = p->next;
		}
		ListElement* l = p->next;
		newel->next = l;
		newel->prev = p;
		p->next = newel;
		if(l != NULL)
			l->prev = newel;
		currentSize++;
	}
	double t = tmr.elapsed();
	
	Timer tmrdelete;
	ListElement * p = root;
	while(p != NULL)
	{	
		ListElement* del = p;
		p = p->next;
		g_mmListDelete.deleteElement(del);
	}
	printf("tmrdelete : %f \n", tmrdelete.elapsed());

	return t;
}

double test_containerCustomMalloc(size_t count)
{
	srand(42);
	Timer tmr;

	ListElement * root = (ListElement*)malloc(sizeof(ListElement));
	root->next = root->prev = NULL;
	ListElement * prev = root;
	size_t currentSize = 1;
	// generate big list
	for (unsigned int i = 0; i < count; i++)
	{
		size_t pos = rand() % currentSize;
		ListElement *newel = (ListElement*)malloc(sizeof(ListElement));
		ListElement * p = root;
		for (unsigned int j = 0; j < pos; j++)
		{
			p = p->next;
		}
		ListElement* l = p->next;
		newel->next = l;
		newel->prev = p;
		p->next = newel;
		if (l != NULL)
			l->prev = newel;
		currentSize++;
	}
	double t = tmr.elapsed();
	Timer tmrdelete;
	ListElement * p = root;
	while (p != NULL)
	{
		ListElement* del = p;
		p = p->next;
		free(del);
	}
	printf("tmrdelete malloc : %f \n", tmrdelete.elapsed());

	return t;
}

template<class T>
double test_container(size_t count)
{
	T container;
	typename T::iterator it;

	srand(42);
	Timer tmr;

	container.push_back(EpicStruct(0));

	for (size_t i = 0; i < count; ++i)
	{
		size_t pos = rand() % container.size();

		it = container.begin();
		for (size_t p = 0; p < pos; ++p)
		{
			it++;
		}

		container.insert(it, EpicStruct(i));
	}

	
	return tmr.elapsed();
}


int main()
{
	size_t count = 100000;
	double t;

	t = test_containerCustomArray(count);
	printf("[Custom MMAnagerArray]Elapsed time mylist: %.2f ms\n", t);

	t = test_containerCustomList(count);
	printf("[Custom MMAnagerList]Elapsed time mylist: %.2f ms\n", t);

	t = test_containerCustomMalloc(count);
	printf("Elapsed time mylist: %.2f ms\n", t);

	t = test_container<vector<EpicStruct>>(count);
	printf("Elapsed time std::vector: %.2f ms\n", t);

	t = test_container<list<EpicStruct>>(count);
	printf("Elapsed time std::list: %.2f ms\n", t);

	return 0;
}