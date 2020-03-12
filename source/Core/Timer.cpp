/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Timer.cpp
 @brief Timer Class
 @author minseob(leeminseob@outlook.com)
 ***********************************************************/

#include <LEPlatform>

#include "Core/Timer.h"

namespace LimitEngine {
#if defined(WINDOWS)
    uint64 Timer::GetTimeUSec()
    {
        LARGE_INTEGER freq_;
        QueryPerformanceFrequency(&freq_);
        
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        
        return counter.QuadPart * 1000000 / freq_.QuadPart;
    }
    double Timer::GetTimeDoubleSecond()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        LARGE_INTEGER cycles;
        QueryPerformanceCounter(&cycles);

        return static_cast<double>(cycles.QuadPart) / static_cast<double>(frequency.QuadPart);
    }
#elif defined(IOS) || defined(ANDROID)
    uint64 Timer::GetTimeUSec()
    {
        struct timeval val;
        struct timezone zone;
        gettimeofday( &val, &zone );
        return val.tv_sec * 1000000 + val.tv_usec;
    }
#endif
}