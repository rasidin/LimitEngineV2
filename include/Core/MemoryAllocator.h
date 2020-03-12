/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  MemoryAllocator.h
 @brief Memory allocator
 @author minseob
 ***********************************************************/
#pragma once

#include <LEPlatform>

#include <stdio.h>
#include <string.h>
#include "Core/Memory.h"
#include "Core/Common.h"

#define MEMORY_BLOCK_SIZE 16
#define MEMORY_BLOCK_ALIGN_BYTES 16

namespace LimitEngine {
    class MemoryAllocator : public Memory
    {
    public:
        struct MEMBLOCKHEADER
        {
            uint32 memBlocks;
            uint8 allocated : 1;
            uint8 padding_flags : 7;
            uint32 category;
            uint32 padding[1];
            
            MEMBLOCKHEADER()
            {
                clear();
            }
            
            void clear()
            {
                ::memset(this,0,sizeof(*this));
            }
            
            uint32 getWholeAreaBlocks() const
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
        };
        
    public:
        static void Init();
        static void InitWithMemoryPool(size_t size);
        static void Term();
        
        static void* Alloc(size_t size, uint32 category = 0);
        static void Free(void* ptr);

    private:
        static MEMBLOCKHEADER* findUnusedMemoryArea( uint32 requiredMemBlocks , bool useCash = false);
        
        static uint32 getMemoryAreasCount();
        static void mergeUnusedMemoryAreaAll();
        
        static const MEMBLOCKHEADER* getMemoryEndAddr()
        {
            return mMemBlocks + mWholeMemBlocksCount;
        }
        
        static void setEnableUnusedMemoryAreaCash( bool enable )
        {
            mIsCacheEnable = enable;
            mCachedUnusedArea = NULL;
        }
        
        static bool isUnusedMemoryAreaCashEnabled()
        {
            return 0 != mIsCacheEnable;
        }
        
        static void                *mPool;                      // Memory pool
        static size_t               mPoolSize;                  // Size of memory pool
        static MEMBLOCKHEADER      *mMemBlocks;                 // Memory blocks
        static uint32               mWholeMemBlocksCount;       // Count of memory blocks
        
        static MEMBLOCKHEADER      *mCachedUnusedArea;          // Cached unused memory blocks
        static uint32               mIsCacheEnable;             // Is cache enabled?
    };
}
