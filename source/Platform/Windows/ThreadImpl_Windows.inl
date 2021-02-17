/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ThreadImpl_Windows.cpp
@brief Thread implementation (windows)
@author minseob (https://github.com/rasidin)
***********************************************************/
#ifdef WINDOWS
#include <process.h>
#include <iostream>

#include "Core/Common.h"
#include "Core/Thread.h"
namespace LimitEngine {
    unsigned int __stdcall ThreadRun(void* arg)
    {
        Thread* thread = reinterpret_cast<Thread*>(arg);
        thread->Run();
        _endthreadex(0);
        return 0;
    }
    THREADHANDLE ThreadImpl::CreateThread(Thread *thread, const ThreadParam &param)
    {
        THREADHANDLE threadHandle = reinterpret_cast<THREADHANDLE>(_beginthreadex(NULL, 0, ThreadRun, thread, 0, NULL));
        ::SetThreadPriority(threadHandle, param.priority);
        if (param.affinityMask != 0) {
            ::SetThreadAffinityMask(threadHandle, param.affinityMask);
            if (param.process >= 0) {
                ::SetThreadIdealProcessor(threadHandle, param.process);
            }
        }
        else if (param.process >= 0) {
            ::SetThreadIdealProcessor(threadHandle, param.process);
        }
        if (param.name.GetLength()) {
            static constexpr uint32 MaxThreadNameCount = 0xff;
            wchar_t threadName[MaxThreadNameCount];
            size_t convertedSize = 0u;
            mbstowcs_s(&convertedSize, threadName, param.name.GetCharPtr(), MaxThreadNameCount);
            ::SetThreadDescription(threadHandle, threadName);
        }
        return threadHandle;
    }
    void ThreadImpl::Join(THREADHANDLE threadhandle)
    {
        WaitForSingleObject(threadhandle, INFINITE);
        CloseHandle(threadhandle);
    }
} // LimitEngine
#endif