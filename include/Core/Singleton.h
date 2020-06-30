/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Singleton.h
 @brief Singleton Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/12 Created by minseob
 ***********************************************************/
#pragma once

#include "Core/Common.h"
#include "Core/Object.h"
#include "Core/Memory.h"

namespace LimitEngine
{
    template <typename T, uint32 category = LimitEngineMemoryCategory_Unknown> class Singleton : public Object<category>
    {
    public:
        typedef T InstanceType;
    
    public:                // Public Functions
        // ==================================
        // Static Functions
        // ==================================
        static T&    GetSingleton()           { LEASSERT(mInstance); return *mInstance; }
        static T*    GetSingletonPtr()        {                      return  mInstance; }
        static bool  IsUsable()               { return mInstance != nullptr; }

        // ==================================
        // CTor & DTor
        // ==================================
        Singleton()                           { LEASSERT(!mInstance); mInstance = static_cast<T*>(this); }
        virtual ~Singleton()                  { LEASSERT(mInstance); mInstance = 0; }

    private:            // NonCopiable
        Singleton(const Singleton<T> &);
        Singleton& operator = (const Singleton<T> &);

    protected:        // Protected Members
        static T* mInstance;

    }; // Singleton
}
