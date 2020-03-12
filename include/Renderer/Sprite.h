/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Sprite.h
 @brief Sprite
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/18 Created by minseob
 ***********************************************************/

#ifndef _LE_SPRITE_H_
#define _LE_SPRITE_H_

#include <LE_Math>

#include "LE_Object.h"
#include "LE_VectorArray.h"

namespace LimitEngine {
    class Texture;
    class SpriteFactory;
    class Sprite : public Object<LimitEngineMemoryCategory_Graphics>
    {
        friend SpriteFactory;
    private:            // Private Structure
        typedef struct _FRAME
        {
            fRect       rect;
            _FRAME() : rect() {}
            _FRAME(const fRect &r)
            {
                rect = r;
            }
            void operator=(const fRect &r) { rect = r; }
        } FRAME;
        
    public:
        Sprite(void *data);
        Sprite(const char *filename);
        Sprite(Texture *tex);
        virtual ~Sprite();
        
        void AddFrame(const iRect &frameRect);
        void GenerateFrame(const iSize &frameSize);      
        void Draw(uint32 frame, const iPoint &pos, float rotation = 0);
        void Draw(uint32 frame, const iRect &rect, float rotation = 0);
        void Draw(const iRect &frame, const iRect &rect, float rotation = 0);
        
        fRect GetFrameRect(uint32 f);
        
    private:
        void LoadFromText(const char *text);

    private:
        Texture             *_texture;
        VectorArray<FRAME>   _frames;
    }; // Sprite
}

#endif
