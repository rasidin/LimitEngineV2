/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Timer.h
 @brief Timer Class
 @author minseob(leeminseob@outlook.com)
 ***********************************************************/

#pragma once

namespace LimitEngine
{
    class Timer
    {
    public:
        static uint64 GetTimeUSec();
        static double GetTimeDoubleSecond();
    };
}
