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


struct ListElement
{
	ListElement * next;
	ListElement * prev;
	EpicStruct  value;
};

typedef  MemoryManagerArray<ListElement, List_SIZE>		MMARRAY;
typedef	 MemoryManagerList<ListElement, List_SIZE>		MMLIST;
typedef  MemoryManagerMalloc<ListElement, List_SIZE>	MMMALLOC;
typedef  MemoryManagerComplete<ListElement, List_SIZE>  MMCOMPLETE;

template <class MemoryManager>
double test_containerCustom(MemoryManager& mmgr)
{
	srand(42);
	Timer tmr;

	ListElement * root = mmgr.getNewElement();
	root->next = root->prev = NULL;
	ListElement * prev = root;
	size_t currentSize = 1;
	// generate big list
	for (unsigned int i = 0; i < TestElements_SIZE; i++)
	{
		size_t pos = rand() % currentSize;
		ListElement *newel = mmgr.getNewElement();
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
	return tmr.elapsed();
}

int main()
{
	MMARRAY    mmArray;
	MMLIST     mmList;
	MMMALLOC   mmMalloc;
	MMCOMPLETE mmComplete;

	double t;
	t = test_containerCustom<MMCOMPLETE>(mmComplete);
	printf("[Custom MMAnagerComplete]Elapsed time mylist: %.2f ms\n", t);

	t = test_containerCustom<MMARRAY>(mmArray);
	printf("[Custom MMAnagerArray]Elapsed time mylist: %.2f ms\n", t);

	t = test_containerCustom<MMLIST>(mmList);
	printf("[Custom MMAnagerList]Elapsed time mylist: %.2f ms\n", t);

	t = test_containerCustom<MMMALLOC>(mmMalloc);
	printf("[Custom MMAnagerMalloc]Elapsed time mylist: %.2f ms\n", t);

	return 0;
}