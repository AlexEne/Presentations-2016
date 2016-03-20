#pragma once
template <class T,unsigned int count>
class MemoryManagerArray
{
	const unsigned int c_blockSize = sizeof(T) * count;

	//pointer to beginging of allocated memory
	T	*m_allocatedMemory;
	unsigned int    m_currentIdx;
public:

	MemoryManagerArray() :m_currentIdx(0)
	{
		m_allocatedMemory = (T*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, c_blockSize);
	}

	T* getNewElement()
	{
		if (m_currentIdx < count)
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