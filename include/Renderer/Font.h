/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Font.h
 @brief Font Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include <LEIntVector2.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/ReferenceCountedPointer.h"
#include "Core/SerializableResource.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
    class Sprite;
    class Font : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>, public SerializableResource
    {
        enum class FileVersion : uint32 {
            FirstVersion = 1,

            CurrentVersion = FirstVersion
        };

    private:
        struct Glyph
        {
            LEMath::IntSize size;
            uint8 ascii;
            uint8 frameIndex;
        };

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
        virtual ~Font();
        
        virtual void InitResource() override;

        void SetConvertTable(uint32 start, uint32 size, char *table = 0);
        
        void Draw(const LEMath::IntPoint &pos, char *text, float size = 1.f);

    public: // Generator
        static Font* GenerateFromFile(const char *ImageFilePath, const char *GlyphFilePath);
        SerializableResource* GenerateNew() const override { return dynamic_cast<SerializableResource*>(new Font()); }

    protected: // Serializer
        virtual bool Serialize(Archive &OutArchive) override;

        virtual uint32 GetFileType() const { return ('F' | ('O' << 8) | ('N' << 16) | ('T' << 24)); }
        virtual uint32 GetVersion() const { return (uint32)FileVersion::CurrentVersion; }

    private:
        ReferenceCountedPointer<Sprite> mSprite;
        VectorArray<Glyph>              mGlyphs;
    };
}
