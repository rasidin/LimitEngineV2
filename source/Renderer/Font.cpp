/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Font.cpp
 @brief Font Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/Font.h"

#include <LEIntVector2.h>

#include "Core/TextParser.h"
#include "Core/Util.h"
#include "Factories/TextParserFactory.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Sprite.h"
#include "Managers/ResourceManager.h"
#include "Managers/ShaderManager.h"

namespace LimitEngine {
    Font* Font::GenerateFromFile(const char *ImageFilePath, const char *GlyphFilePath)
    {
        Font *Output = new Font();

        // Make sprite file
        Output->mSprite = Sprite::GenerateFromFile(ImageFilePath, nullptr);

        // Set sprite frames from file
        AutoPointer<ResourceManager::RESOURCE> textResource = LE_ResourceManager.GetResourceWithoutRegister(GlyphFilePath, TextParserFactory::ID);
        if (textResource.Exists()) {
            if (TextParser *parser = dynamic_cast<TextParser*>(((ResourceManager::RESOURCE*)textResource)->data))
            {
                TextParser::NODE *node = NULL;
                if ((node = parser->GetNode("FILETYPE")) && node->values[0] == "FONT")
                {
                    if (TextParser::NODE *datanode = parser->GetNode("DATA"))
                        if (node = datanode->children[0])
                        {
                            Output->mGlyphs.Reserve(node->children.count());
                            for (uint32 i = 0; i < node->children.count(); i++)
                            {
                                if (TextParser::NODE *child = node->children[i]) {
                                    Font::Glyph &newGlyph = Output->mGlyphs.Add();
                                    if (TextParser::NODE *node_rect = child->FindChild("RECT"))
                                    {
                                        LEMath::IntRect frameRect(
                                            node_rect->values[0].ToInt(),
                                            node_rect->values[1].ToInt(),
                                            node_rect->values[2].ToInt(),
                                            node_rect->values[3].ToInt()
                                        );
                                        newGlyph.frameIndex = Output->mSprite->AddFrame(frameRect);
                                        newGlyph.size = LEMath::IntVector2(frameRect.Width(), frameRect.Height());
                                    }
                                    if (TextParser::NODE *node_ascii = child->FindChild("ASCII"))
                                    {
                                        newGlyph.ascii = node_ascii->values[0].ToInt();
                                    }
                                }
                            }
                        }
                }
            }
        }
        return Output;
    }
    Font::Font()
    {
    }
    Font::~Font()
    {
        mSprite.Release();
    }
    void Font::InitResource() {
        if (mSprite.IsValid()) {
            mSprite->InitResource();
        }
    }
    bool Font::Serialize(Archive &OutArchive) {
        OutArchive << (SerializableResource*)&mGlyphs;
        if (mSprite.IsValid() == false) {
            mSprite = new Sprite();
        }
        OutArchive << (SerializableResource*)mSprite.Get();
        return true;
    }

/*
    void Font::SetConvertTable(uint32 start, uint32 size, char *table)
    {
        if (mTable.table) mTable.~_CONVERT_TABLE();
        mTable.start = start;
        mTable.size = size;
        if (table) mTable.table = table;
        else {
            mTable.table = (char *)malloc(mTable.size);
            for (uint32 i=0;i<size;i++) mTable.table[i] = i;
        }
    }
*/
    void Font::Draw(const LEMath::IntPoint &pos, char *text, float size)
    {
        if (!mSprite.IsValid() || mGlyphs.count() == 0) return;

        //DrawCommand::SetDisable((uint32)RendererFlag::EnabledFlags::DEPTH_TEST);
        //DrawCommand::SetDisable((uint32)RendererFlag::EnabledFlags::DEPTH_WRITE);
        //DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::ALPHABLEND);

        ShaderRefPtr FontShader = LE_ShaderManager.GetShader("DrawFont");

        mSprite->BeginBatchDraw();

        LEMath::IntPoint currentPosition = pos;
        const char glyphAsciiStart = mGlyphs[0].ascii;
        char *ptr = text;
        while (*ptr)
        {
            if (glyphAsciiStart > *ptr && (*ptr) - glyphAsciiStart >= static_cast<char>(mGlyphs.count())) {
                ptr++;
                continue;
            }

            uint8 glyphIndex = (*ptr) - glyphAsciiStart;
            mSprite->BatchDraw(glyphIndex, currentPosition, Sprite::CoordinateType::RealCoordinate);

            ptr++;
            currentPosition += LEMath::IntPoint(mGlyphs[glyphIndex].size.X(), 0);
        }

        mSprite->EndBatchDraw(FontShader.Get());

        //DrawCommand::SetEnable((uint32)RendererFlag::EnabledFlags::DEPTH_TEST);
        //DrawCommand::SetEnable((uint32)RendererFlag::EnabledFlags::DEPTH_WRITE);
    }
}