//
//  LE_MemoryManager.h
//  LimitEngine
//
//  Created by hikaru on 2014/02/23.
//
//

#ifndef _LE_MEMORYMANAGER_H_
#define _LE_MEMORYMANAGER_H_

#include "LE_Singleton.h"
#include "LE_Memory.h"

namespace LimitEngine {
    class MemoryManager : public Singleton<MemoryManager>
    {
    public:
        MemoryManager() {}
        virtual ~MemoryManager() {}
        
        static void Init();
        static void InitWithMemoryPool(size_t poolSize);
        
        static void* Alloc(size_t size)
        {
          return Memory::Malloc(size);
        }
        static void Free(void* ptr)
        {
            Memory::Free(ptr);
        }
        
        static void Term();
    };
}


#endif
