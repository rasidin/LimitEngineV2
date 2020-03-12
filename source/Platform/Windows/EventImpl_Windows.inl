/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  EventImpl_Windows.h
@brief Implementation for Event (Windows)
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifdef WINDOWS
namespace LimitEngine {
Event::Event(const char *name, bool initState, bool autoReset)
{
    mHandle = ::CreateEvent(NULL, !autoReset, initState, name);
}
Event::~Event()
{
    if (mHandle)
        ::CloseHandle(mHandle);
}

void Event::Wait(uint32 milliSecond)
{
    ::WaitForSingleObject(mHandle, milliSecond);
}
bool Event::TryWait()
{
    DWORD result = ::WaitForSingleObject(mHandle, 0);
    return result == WAIT_OBJECT_0;
}
void Event::Signal()
{
    ::SetEvent(mHandle);
}
void Event::Reset()
{
    ::ResetEvent(mHandle);
}
}
#endif