#pragma once
#include <stdio.h>
#include "defines.h"

struct EpicStruct
{
	char m_memory[EpicStruct_SIZE];

	EpicStruct()
	{
	}

	explicit EpicStruct(size_t val)
	{
		memset(m_memory, val % 127, sizeof(m_memory));
	}
	
	~EpicStruct()
	{
	}

	void print()
	{
		for (int i = 0; i < EpicStruct_SIZE; ++i)
			printf("%d", m_memory[i]);
	}
};