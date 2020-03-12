/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_Memory.cpp
 @brief Memory Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include <stdlib.h>
#include "Core/Memory.h"

namespace LimitEngine
{
	void* Memory::Malloc(size_t size)
	{
		return ::malloc(size);
	}

	void Memory::Free(void *data)
	{
		::free(data);
	}
}