/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Data.h
 @brief Default Data Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#pragma once

#include "Core/Common.h"

namespace LimitEngine {
    class Data
    {
    public:
        static const uint16 MagicWord = ('L') | ('E' << 8);
        
        enum TYPE
        {
            TYPE_AUTO = 0,
            TYPE_TEXTURE = ('T') | ('X' << 8),
            TYPE_SPRITE  = ('S') | ('P' << 8),
            TYPE_MODEL   = ('M') | ('D' << 8),
            TYPE_MENU    = ('M') | ('N' << 8),
            TYPE_EFFECT  = ('E') | ('F' << 8),
            TYPE_MAX,
        };
        
        typedef struct
        {
            uint16      magic;
            uint16      type;
        } HEADER;
    };
}
