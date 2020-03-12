/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Memory.h
 @brief Memory super class (for overwriting malloc and free...)
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/12 Created by minseob
 ***********************************************************/
#pragma once

#ifdef WINDOWS
#include <malloc.h>
#endif

namespace LimitEngine
{
	enum LimitEngineMemoryCategory {
		LimitEngineMemoryCategory_Unknown = 0,

		LimitEngineMemoryCategory_Common,
		LimitEngineMemoryCategory_Debug,
		LimitEngineMemoryCategory_Form,
		LimitEngineMemoryCategory_Graphics,

		LimitEngineMemoryCategory_Count,
	};
    static const char* LimitEngineMemoryCategoryName[] = {
        "Unknown",

        "Common",
        "Debug",
        "Form",
        "Graphics",
    };
class Memory
{
protected:
	static void* Malloc(size_t size);
	static void Free(void *data);
}; // Memory
}
