#pragma once
template <class T,unsigned int count>
class MemoryManagerArray
{
	//pointer to beginging of allocated memory
	T	*m_allocatedMemory;
	unsigned int    m_currentIdx;
public:
	MemoryManagerArray() :m_currentIdx(0)
	{
		//allocate all the memory :)
		m_allocatedMemory = (T*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(T) * count);
	}

	T* getNewElement()
	{
		return (m_currentIdx < count) ? &m_allocatedMemory[m_currentIdx++] : nullptr;
	}
	void deleteElement(T * el) const{//NOTHING}
	~MemoryManagerArray()
	{
		//Free all the memory
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, m_allocatedMemory);
	}
};
