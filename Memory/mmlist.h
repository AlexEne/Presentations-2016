#pragma once
// NO NON-POD Classes supported (because of offsetof)
template <class T,unsigned int count>
class MemoryManagerList
{
	const unsigned int c_blockSize = sizeof(MemElement) * count;

	//pointer to beginging of allocated memory
	unsigned char	*m_allocatedMemory;
	//value field offset in structure
	unsigned int	m_structValueOffset;
	//List of all memory blocks 
	struct MemElement
	{
		MemElement *next;
		T val;
	} *m_freeList;

public:

	MemoryManagerList()
	{
		m_allocatedMemory = (unsigned char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, c_blockSize);

		m_freeList = (MemElement*)m_allocatedMemory;
		MemElement * listIter = m_freeList;
		// link the free list elements, they are first held in contigous memory
		for (unsigned int i = 0; i < count; i++)
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

	void deleteElement(T * el)
	{
		size_t adress = (size_t)el;
		MemElement * melAddr = (MemElement*)(adress - m_structValueOffset);
		melAddr->next = m_freeList;
		m_freeList = melAddr;
	}

	~MemoryManagerList()
	{
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, m_allocatedMemory);
	}

};