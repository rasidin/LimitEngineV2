/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Sprite.h
 @brief Sprite
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include <LEIntVector2.h>
#include <LEIntVector4.h>
#include <LEFloatVector4.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/ReferenceCountedPointer.h"
#include "Containers/VectorArray.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
    class Texture;
    class SpriteFactory;
    class Sprite : public ReferenceCountedObject<LimitEngineMemoryCategory_Graphics>, public SerializableResource
    {
        friend SpriteFactory;
    private:            // Private Structure
        typedef struct _FRAME
        {
            LEMath::FloatRect mRect;
            _FRAME() : mRect() {}
            _FRAME(const LEMath::FloatRect &r)
            {
                mRect = r;
            }
            void operator=(const LEMath::FloatRect &r) { mRect = r; }
        } FRAME;

    public:
        enum class CoordinateType {
            VirtualCoordinate = 0,
            RealCoordinate
        };

    public:
        Sprite();
        virtual ~Sprite();
        
        virtual void InitResource() override;

        void SetTexture(Texture *InTexture) { mTexture = InTexture; }

        uint32 AddFrame(const LEMath::IntRect &frameRect);
        void GenerateFrame(const LEMath::IntSize &frameSize);      
        void Draw(uint32 frame, const LEMath::IntPoint &pos, float rotation = 0);
        void Draw(uint32 frame, const LEMath::IntRect &rect, float rotation = 0);
        void Draw(const LEMath::IntRect &frame, const LEMath::IntRect &rect, float rotation = 0);

        void BeginBatchDraw();
        void BatchDraw(uint32 frame, const LEMath::IntPoint &pos, CoordinateType CoordType = CoordinateType::VirtualCoordinate);
        void EndBatchDraw(Shader *InShader = nullptr);

        LEMath::FloatRect GetFrameRect(uint32 f);

        virtual bool Serialize(Archive &OutArchive) override;

    public: // Generators
        static Sprite* GenerateFromFile(const char *ImageFilePath, const char *InfoFilePath);

    private:
        void LoadFromText(const char *text);

    private:
        ReferenceCountedPointer<Texture>    mTexture;
        VectorArray<FRAME>                  mFrames;
    }; // Sprite
    typedef ReferenceCountedPointer<Sprite> SpriteReferencePointer;
}
