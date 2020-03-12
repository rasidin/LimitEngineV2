/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_Event.h
@brief Event for multithread
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2017/4/20 Created by minseob
***********************************************************/
#pragma once
#ifndef _LE_EVENT_H_
#define _LE_EVENT_H_

#include "Core/Common.h"

namespace LimitEngine {
class Event
{
public:
    Event(const char *name = NULL, bool initState = false, bool autoReset = true);
    virtual ~Event();

    void Wait(uint32 milliSecond = 0x7fffffff);
    bool TryWait();
    void Signal();
    void Reset();
private:
    EVENT_HANDLE mHandle;
};
}

#endif // _LE_EVENT_H_
