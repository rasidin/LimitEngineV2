/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_Thread.h
@brief Thread
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2017/4/19 Created by minseob
***********************************************************/
#pragma once

#include "Core/Common.h"
#include "Core/Object.h"
#include "Core/Function.h"
#include "Core/String.h"

namespace LimitEngine {
    class Thread;
    typedef Function<void> ThreadFunction;
    struct ThreadParam
    {
        ThreadParam()
            : func(NULL)
            , name()
            , process(-1)
            , priority(5)
            , affinityMask(0)
        {}
        ThreadFunction func;
        String name;
        uint32 affinityMask;
        uint32 priority;
        int32 process;
    };
    class ThreadImpl
    {
    public:
        static HANDLE CreateThread(Thread *thread, const ThreadParam &param);
        static void Join(THREADHANDLE threadhandle);
    };
    class Thread : public Object<LimitEngineMemoryCategory::Common>
    {
    public:
        Thread()
        : mFunc()
        , mHandle(0)
        {
        }
        
        ~Thread(){}
        
        bool IsRunning() const
        {
            return mHandle == NULL ? false : true;
        }
        
        bool Create(const ThreadParam &param)
        {
            mFunc = param.func;
            mHandle = ThreadImpl::CreateThread(this, param);

            if (mHandle == 0)
                return false;
            return true;
        }
        
        bool Join()
        {
            ThreadImpl::Join(mHandle);
            mHandle = NULL;
            return true;
        }

        void Run() {
            mFunc();
        }

    private:
        ThreadFunction mFunc;
        THREADHANDLE mHandle;
    };
}
