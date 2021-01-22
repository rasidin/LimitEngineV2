/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file MemoryAllocator.h
@brief Memory allocator
@author minseob
**********************************************************************/
#ifndef LIMITENGINEV2_CORE_MEMORYALLOCATOR_H_
#define LIMITENGINEV2_CORE_MEMORYALLOCATOR_H_

#include <LEPlatform>

#include <stdio.h>
#include <string.h>
#include "Core/Memory.h"
#include "Core/Common.h"

namespace LimitEngine {
class MemoryAllocator : public Memory
{
    static constexpr size_t MemoryBlockSize = 16u;

    static struct STATS
    {
        size_t AllocatedMemory = 0u;
    } mStats;

public:
    struct MEMBLOCKHEADER
    {
        uint64 memBlocks;           // [8]
        uint32 category;            // [4]
        uint8  allocated : 1;       // [1]
        uint8  padding_flags : 7;
        uint8  _[3];                // [3]

        MEMBLOCKHEADER()
        {
            clear();
        }

        void clear()
        {
            ::memset(this, 0, sizeof(*this));
        }

        size_t getWholeAreaBlocks() const
        {
            return memBlocks + 1;
        }

        MEMBLOCKHEADER* getNextMemoryArea()
        {
            return (this + 1) + memBlocks;
        }

        MEMBLOCKHEADER* splitMemoryArea(uint32 splitBlocks);

        bool canMergeNextMemoryArea();
        bool mergeNextMemoryArea();
    }; // 16 bytes

public:
    static void Init();
    static void InitWithMemoryPool(size_t size);
    static void Term();
        
    static void* Alloc(size_t size, LimitEngineMemoryCategory category = LimitEngineMemoryCategory::Unknown);
    static void Free(void* ptr);

    static size_t GetStatAllocatedMemory() { return mStats.AllocatedMemory; }

private:
    static MEMBLOCKHEADER* findUnusedMemoryArea(size_t requiredMemBlocks);

    static uint32 getMemoryAreasCount();
    static void mergeUnusedMemoryAreaAll();

    static const MEMBLOCKHEADER* getMemoryEndAddr()
    {
        return mMemBlocks + mWholeMemBlocksCount;
    }
        
    static void                *mPool;                      // Memory pool
    static size_t               mPoolSize;                  // Size of memory pool
    static MEMBLOCKHEADER      *mMemBlocks;                 // Memory blocks
    static uint32               mWholeMemBlocksCount;       // Count of memory blocks
};
}
#endif // LIMITENGINEV2_CORE_MEMORYALLOCATOR_H_