/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_Object.h
 @brief Object Class (BaseObject) Overwrite function for memory
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Core/Memory.h"
#include "Core/MemoryAllocator.h"

namespace LimitEngine {
template<LimitEngineMemoryCategory category = LimitEngineMemoryCategory::Unknown>
class Object
{public:
	virtual ~Object(){}
	void* operator new (size_t size)
	{
		return MemoryAllocator::Alloc(size, category);
	}
	void* operator new[] (size_t size)
	{
		return MemoryAllocator::Alloc(size, category);
	}
	void operator delete (void *data)
	{
        MemoryAllocator::Free(data);
	}
	void operator delete[] (void *data)
	{
		MemoryAllocator::Free(data);
	}
	void* malloc(size_t size) const
	{
		return MemoryAllocator::Alloc(size, category);
	}
	void free(void *data) const
	{
		MemoryAllocator::Free(data);
	}
};
}
