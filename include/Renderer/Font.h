/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Font.h
 @brief Font Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/9/15 Created by minseob
 ***********************************************************/

#ifndef _LE_FONT_H_
#define _LE_FONT_H_

#include <LE_Math>
#include "LE_Object.h"

namespace LimitEngine {
    class Sprite;
    class Font : public Object<LimitEngineMemoryCategory_Graphics>
    {
    private:
        typedef struct _CONVERT_TABLE
        {
            uint32       start;
            uint32       size;
            char        *table;
            char Convert(const char t) 
            { 
                if (table && t >= int(start) && t < int(start + size)) return table[t-start]; 
                return 0; 
            }
            _CONVERT_TABLE()
            {
                start = 0;
                size = 0;
                table = 0;
            }
            ~_CONVERT_TABLE()
            {
                if (table) MemoryAllocator::Free(table);
                table = 0;
            }
        } CONVERT_TABLE;
        
    public:
        Font();
        Font(const char *filename);
        virtual ~Font();
        
        void SetSprite(Sprite *s)       { mSprite = s; }
        void SetConvertTable(uint32 start, uint32 size, char *table = 0);
        
        void Draw(const iPoint &pos, char *text, float size = 1.f);
        
    private:
        CONVERT_TABLE            mTable;
        Sprite                  *mSprite;
    };
}

#endif // _LE_FONT_H_
