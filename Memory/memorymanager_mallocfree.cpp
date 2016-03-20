#include <list>
#include <vector>
#include <random>
#include <stdio.h>
#include <assert.h>
#include <windows.h>

#include "defines.h"

#include "simpletimer.h"
#include "epicstruct.h"
#include "mmarray.h"
#include "mmlist.h"
#include "mmcomplete.h"
#include "mmmalloc.h"
using namespace std;

typedef  MemoryManagerArray<EpicStruct, List_SIZE>		MMARRAY;
typedef	 MemoryManagerList<EpicStruct, List_SIZE>		MMLIST;
typedef  MemoryManagerMalloc<EpicStruct, List_SIZE>	    MMMALLOC;
typedef  MemoryManagerComplete<EpicStruct, List_SIZE>   MMCOMPLETE;


template <class MemoryManager>
double test_MManagerAllocDealloc(MemoryManager& mmgr)
{
	Timer tmr;
	unsigned int i;
	std::vector<EpicStruct*> appPtrs;
	appPtrs.resize(List_SIZE);

	for (i = 0; i < List_SIZE; i++)
	{
		appPtrs[i] = mmgr.getNewElement();
	}
	for (i = 0; i < List_SIZE; i++)
	{
		mmgr.deleteElement(appPtrs[i]);
	}

	return tmr.elapsed();
}

int main()
{
	MMARRAY    mmArray;
	MMLIST     mmList;
	MMMALLOC   mmMalloc;
	MMCOMPLETE mmComplete;

	double t;
	t = test_MManagerAllocDealloc<MMCOMPLETE>(mmComplete);
	printf("[Custom MMAnagerComplete]Elapsed time alloc/dealloc: %.2f ms\n", t);

	t = test_MManagerAllocDealloc<MMARRAY>(mmArray);
	printf("[Custom MMAnagerArray]Elapsed time alloc/dealloc: %.2f ms\n", t);

	t = test_MManagerAllocDealloc<MMLIST>(mmList);
	printf("[Custom MMAnagerList]Elapsed time alloc/dealloc: %.2f ms\n", t);
	
	t = test_MManagerAllocDealloc<MMMALLOC>(mmMalloc);
	printf("[Custom MMAnagerMalloc]Elapsed time alloc/dealloc: %.2f ms\n", t);

	return 0;
}