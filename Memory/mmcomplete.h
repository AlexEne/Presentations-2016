#pragma once
template <class T,int numBlocks>
class MemoryManagerComplete
{
	unsigned int m_numOfBlocks; // Number of blocks
	unsigned int m_numFreeBlocks; // Num of remaining blocks
	unsigned int m_numInitialized; // Num of initialized blocks
	unsigned char* m_memStart; // Begining of memory
	unsigned char* m_next; // num of next free block
public:
	MemoryManagerComplete()
	{
		m_numOfBlocks	= numBlocks;
		m_memStart		= (unsigned char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(T) * m_numOfBlocks); 
		m_numFreeBlocks = numBlocks;
		m_next			= m_memStart;
	}

	~MemoryManagerComplete()
	{
		Destroy();
	}

	void Destroy()
	{
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, m_memStart);
	}

	inline unsigned char * AddrFromIndex(unsigned int i)const
	{
		return m_memStart + (i * sizeof(T));
	}

	inline unsigned int IndexFromAddr(const unsigned char * p)const
	{
		return ((unsigned int)(p - m_memStart)) / sizeof(T);
	}

	void * Allocate()
	{
		if (m_numInitialized < m_numOfBlocks)
		{
			unsigned int *p = (unsigned int *)AddrFromIndex(m_numInitialized);
			*p = m_numInitialized + 1;
			m_numInitialized++;
		}
		void * ret = nullptr;
		if (m_numFreeBlocks > 0)
		{
			ret = (void*)m_next;
			--m_numFreeBlocks;
			m_next = (m_numFreeBlocks != 0) ? AddrFromIndex(*((unsigned int*)m_next)) : nullptr;
		}
		return ret;
	}

	void DeAllocate(void *p)
	{
		(*(unsigned int *)p) = (m_next != nullptr) ? IndexFromAddr(m_next) : m_numOfBlocks;
		m_next = (unsigned char*)p;
		++m_numFreeBlocks;
	}

	T* getNewElement()
	{
		return (T*)Allocate();
	}

	void deleteElement(T * el) 
	{
		DeAllocate(el);
	}
};