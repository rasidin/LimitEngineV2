/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  DebugImpl_Windows.cpp
 @brief for Debug
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#include "Core/Debug.h"

#include <iostream>

#ifdef WINDOWS
namespace LimitEngine {
    class DebugImpl_Windows : public DebugImpl
    {
    public:
        void PrintLog(const char *s)
        {
            OutputDebugString(s);
        }
        void PrintGUI(const char *s)
        {
        }
        void Clear()
        {
        }
    };
}
#endif