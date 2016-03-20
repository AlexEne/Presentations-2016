#pragma once
#pragma once

template <class T,int numblocks>
class MemoryManagerMalloc
{
public:
	T* getNewElement()
	{
		return (T*)malloc(sizeof(T));
	}

	void deleteElement(T * el)
	{
		free(el);
	}
};