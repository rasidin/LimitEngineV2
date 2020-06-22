/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  MemoryAllocator.cpp
 @brief Memory allocator
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Core/MemoryAllocator.h"
#include "Core/Mutex.h"

namespace LimitEngine {
    static Mutex gMutexForMemoryAllocator;

    void* MemoryAllocator::mPool = NULL;
    size_t MemoryAllocator::mPoolSize = 0;
    
    MemoryAllocator::MEMBLOCKHEADER* MemoryAllocator::mMemBlocks = NULL;
    uint32 MemoryAllocator::mWholeMemBlocksCount = 0;
    MemoryAllocator::MEMBLOCKHEADER* MemoryAllocator::mCachedUnusedArea = NULL;
    uint32 MemoryAllocator::mIsCacheEnable = true;

    //!---- MEMBLOCKHEADER
    MemoryAllocator::MEMBLOCKHEADER* MemoryAllocator::MEMBLOCKHEADER::splitMemoryArea(uint32 splitBlocks)
    {
        if ( this->memBlocks <= splitBlocks )
            return NULL;
        
        const uint32 secondAreaWholeBlocks = memBlocks - splitBlocks;
        LEASSERT( 1 <= secondAreaWholeBlocks );
        
        this->memBlocks = splitBlocks;

        MEMBLOCKHEADER* secondBlock = this->getNextMemoryArea();
        secondBlock->clear();
        secondBlock->memBlocks = secondAreaWholeBlocks - 1;
        
        return secondBlock;
    }
    
    bool MemoryAllocator::MEMBLOCKHEADER::canMergeNextMemoryArea()
    {
        MEMBLOCKHEADER* nextArea = this->getNextMemoryArea();
        return (0 == nextArea->allocated);
    }
    
    bool MemoryAllocator::MEMBLOCKHEADER::mergeNextMemoryArea()
    {
        if ( ! this->canMergeNextMemoryArea() )
            return false;
        
        MEMBLOCKHEADER* nextArea = this->getNextMemoryArea();
        this->memBlocks += nextArea->getWholeAreaBlocks();
        
        LEASSERT( this->getNextMemoryArea() == nextArea->getNextMemoryArea() );
        
        nextArea->clear();
        
        return true;
    }
    
    //!---- Allocator
    void MemoryAllocator::Init()
    {
        mPool = NULL;
        mPoolSize = NULL;
        mMemBlocks = NULL;
        mWholeMemBlocksCount = 0;
        mCachedUnusedArea = NULL;
        mIsCacheEnable = false;
    }
    
    void MemoryAllocator::InitWithMemoryPool(size_t size)
    {
        mPool = Memory::Malloc(size);
        mPoolSize = size;
        
        LEASSERT( (mPool != NULL) && "allocate memory pool failed!!!");
        
        mMemBlocks = reinterpret_cast< MEMBLOCKHEADER* >( mPool );
        mWholeMemBlocksCount = static_cast<uint32>(size / MEMORY_BLOCK_SIZE);
        LEASSERT( 1 <= mWholeMemBlocksCount );
        
        MEMBLOCKHEADER* header = mMemBlocks;
        header->clear();
        header->memBlocks = mWholeMemBlocksCount - 1;
        
        mCachedUnusedArea = NULL;
        mIsCacheEnable = true;
    }
    
    void* MemoryAllocator::Alloc(size_t size, uint32 category)
    {
        Mutex::ScopedLock scopedLock(gMutexForMemoryAllocator);
        if( mPool == NULL )
        {
            return Memory::Malloc(size);
        }
        else
        {
            const uint32 newMemBlocks = static_cast<uint32>((size + (MEMORY_BLOCK_SIZE-1)) / MEMORY_BLOCK_SIZE);

            const bool useCache = isUnusedMemoryAreaCashEnabled();
            MEMBLOCKHEADER* memAreaUnused = findUnusedMemoryArea( newMemBlocks, useCache );
            if ( NULL == memAreaUnused )
            {
                if ( useCache )
                {
                    mCachedUnusedArea = NULL;
                    memAreaUnused = findUnusedMemoryArea( newMemBlocks, false );
                    if ( NULL == memAreaUnused ){
                        DEBUG_MESSAGE("[MemoryAllocator] not enough memory!!!!!!!!!");
                        return NULL;
                    }
                }
            }

			MEMBLOCKHEADER* nextArea = memAreaUnused->splitMemoryArea(newMemBlocks);
            memAreaUnused->allocated = true;
			memAreaUnused->category = category;

            if ( mIsCacheEnable )
            {
                if ( NULL != nextArea )
                {
                    mCachedUnusedArea = ( ! nextArea->allocated ) ? nextArea : NULL;
                }
            }
            void* retAddr = memAreaUnused + 1;
			return retAddr;
        }
    }
    
    void MemoryAllocator::Free(void* ptr)
    {
        Mutex::ScopedLock scopedLock(gMutexForMemoryAllocator);
        if( mPool == NULL)
            Memory::Free(ptr);
        else
        {
            if ( NULL == ptr )
                return;
            
            LEASSERT( (0 == (size_t)ptr % MEMORY_BLOCK_ALIGN_BYTES) );
            
            MEMBLOCKHEADER* memArea = reinterpret_cast< MEMBLOCKHEADER* >( ptr ) - 1;
            
            LEASSERT( memArea->allocated && "[MemoryAllocator][Free]double free??");
            
            memArea->allocated = false;
            mCachedUnusedArea = NULL;
            
            mergeUnusedMemoryAreaAll();
        }
    }
    
    void MemoryAllocator::Term()
    {
        if (mPool == NULL)
            return;
        
        mergeUnusedMemoryAreaAll();
		uint32 memoryAreasCount = getMemoryAreasCount();
        LEASSERT( (1 == memoryAreasCount) && "[MemoryAllocator::Term] Leak!!!" );
        
        mMemBlocks           = NULL;
        mWholeMemBlocksCount = 0;
        mCachedUnusedArea = NULL;
        
        Memory::Free(mPool);
        mPool = NULL;
        mPoolSize = 0;
    }
    
    MemoryAllocator::MEMBLOCKHEADER* MemoryAllocator::findUnusedMemoryArea( uint32 requiredMemBlocks , bool useCache)
    {
        MEMBLOCKHEADER* firstFindArea = ( useCache && mCachedUnusedArea ) ? mCachedUnusedArea : mMemBlocks;

        MEMBLOCKHEADER* curAreaHeader = firstFindArea;
        const MEMBLOCKHEADER* endAddr = getMemoryEndAddr();
        while ( true )
        {
            if ( (! curAreaHeader->allocated) && (requiredMemBlocks <= curAreaHeader->memBlocks) )
            {
                return curAreaHeader;
            }
            
            curAreaHeader = curAreaHeader->getNextMemoryArea();
            if ( endAddr <= curAreaHeader ){
                break;
            }
        }
        return NULL;
    }
    
    uint32 MemoryAllocator::getMemoryAreasCount()
    {
       	uint32 areasCount = 0;
        MEMBLOCKHEADER* curArea = mMemBlocks;
        const MEMBLOCKHEADER* memEnd = getMemoryEndAddr();
        while ( curArea < memEnd )
        {
            ++ areasCount;
            curArea = curArea->getNextMemoryArea();
            if (curArea->allocated) {
                DEBUG_MESSAGE("[MemoryAllocator] MemoryLeak : category %s size %d address %llx (%llx)\n", LimitEngineMemoryCategoryName[curArea->category], curArea->memBlocks * MEMORY_BLOCK_SIZE, intptr_t(curArea+1) - intptr_t(mPool), intptr_t(curArea + 1));
            }
        }
        
        return areasCount;
    }
    
    void MemoryAllocator::mergeUnusedMemoryAreaAll()
    {
        mCachedUnusedArea = NULL;
        
        MEMBLOCKHEADER* curArea = mMemBlocks;
        const MEMBLOCKHEADER* memEnd = getMemoryEndAddr();
        while ( curArea < memEnd )
        {
            if ( (NULL == mCachedUnusedArea) && mIsCacheEnable )
            {
                if ( ! curArea->allocated ){
                    mCachedUnusedArea = curArea;
                }
            }
            
            if ( memEnd <= curArea->getNextMemoryArea() ) break;
            
            const bool canMergeNext = (!curArea->allocated) && curArea->canMergeNextMemoryArea();
            
            if ( canMergeNext )
            {
                LEASSERT( !curArea->allocated );
                curArea->mergeNextMemoryArea();
            }
            else
            {
                curArea = curArea->getNextMemoryArea();
            }
        }
    }
}