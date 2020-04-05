/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Draw2DManager.cpp
 @brief DrawManager Class (2D)
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>

#include "Managers/Draw2DManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/ByteColorRGBA.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/RenderContext.h"

namespace LimitEngine {
    Draw2DManager* Draw2DManager::mInstance = nullptr;
    Draw2DManager::Draw2DManager()
        : SingletonDraw2DManager()
        , mShader_draw2d(nullptr)
        , mNullTexture(nullptr)
        , mVertex_draw2d_used(0u)
        , mVertex_draw2d_start(0u)
    {
    }

    void Draw2DManager::Init()
    {
        mShader_draw2d = ShaderManager::GetSingleton().GetShader("Draw2D");
        mVertexbuffer_draw2d.Create(Vertex2DReserved, NULL, static_cast<uint32>(RendererFlag::CreateBufferFlags::CPU_READABLE) | static_cast<uint32>(RendererFlag::CreateBufferFlags::CPU_WRITABLE));

        // Create vertex buffer for drawing full screen (For postfilter, background...)
        Vertex<VERTEXTYPE(PCT)> scrVertex[6];
        scrVertex[0].SetPosition(LEMath::FloatVector3(0.0f, 0.0f, 0.0f));
        scrVertex[1].SetPosition(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
        scrVertex[2].SetPosition(LEMath::FloatVector3(0.0f, 1.0f, 0.0f));
        scrVertex[3].SetPosition(LEMath::FloatVector3(0.0f, 1.0f, 0.0f));
        scrVertex[4].SetPosition(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
        scrVertex[5].SetPosition(LEMath::FloatVector3(1.0f, 1.0f, 0.0f));
        scrVertex[0].SetColor(ByteColorRGBA(0xffffffff));
        scrVertex[1].SetColor(ByteColorRGBA(0xffffffff));
        scrVertex[2].SetColor(ByteColorRGBA(0xffffffff));
        scrVertex[3].SetColor(ByteColorRGBA(0xffffffff));
        scrVertex[4].SetColor(ByteColorRGBA(0xffffffff));
        scrVertex[5].SetColor(ByteColorRGBA(0xffffffff));
        scrVertex[0].SetTexcoord(LEMath::FloatVector2(0.0f, 0.0f));
        scrVertex[1].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
        scrVertex[2].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
        scrVertex[3].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
        scrVertex[4].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
        scrVertex[5].SetTexcoord(LEMath::FloatVector2(1.0f, 1.0f));
        mVertexbuffer_drawscr.Create(6, &scrVertex[0], 0);
        mNullTexture = RenderContext::GetSingleton().GetDefaultTexture(RenderContext::DefaultTextureTypeWhite);
    }
    void Draw2DManager::Term()
    {
    }
    void Draw2DManager::EndOfFrame()
    {
        mVertex_draw2d_start = mVertex_draw2d_used = 0u;
    }
    void Draw2DManager::DrawScreen(Shader *Shader /*= nullptr*/)
    {
        DrawCommand::SetCulling(static_cast<uint32>(RendererFlag::Culling::ClockWise));
        if (Shader == nullptr) {
            if (!mShader_draw2d)
                mShader_draw2d = ShaderManager::GetSingleton().GetShader("Draw2D");
            if (mShader_draw2d) DrawCommand::BindShader(mShader_draw2d);
        }
        else
            DrawCommand::BindShader(Shader);
        mVertexbuffer_drawscr.BindToDrawManager();
        DrawCommand::DrawPrimitive(RendererFlag::PrimitiveTypes::TRIANGLELIST, mVertex_draw2d_start, 6);
    }
    void Draw2DManager::FlushDraw2D(const RendererFlag::PrimitiveTypes &PrimitiveType, Shader *Shader /*= nullptr*/)
    {
        DrawCommand::SetCulling(static_cast<uint32>(RendererFlag::Culling::CounterClockWise));
        if (Shader == nullptr) {
            if (!mShader_draw2d)
                mShader_draw2d = ShaderManager::GetSingleton().GetShader("Draw2D");
            if (mShader_draw2d) DrawCommand::BindShader(mShader_draw2d);
        }
        else
            DrawCommand::BindShader(Shader);
        mVertexbuffer_draw2d.BindToDrawManager();
        DrawCommand::DrawPrimitive(PrimitiveType, mVertex_draw2d_start, mVertex_draw2d_used - mVertex_draw2d_start);
        mVertex_draw2d_start = mVertex_draw2d_used;
    }
    // Get vertex buffer from temporary buffer
    Vertex2D* Draw2DManager::GetVertexBuffer2D(uint32 size)
    {
        LEASSERT(mVertex_draw2d_used + size < mVertexbuffer_draw2d.GetSize());
        if (mVertex_draw2d_used + size > mVertexbuffer_draw2d.GetSize()) return 0;
        size_t startPos = mVertex_draw2d_used;
        mVertex_draw2d_used += size;

        return &mVertexbuffer_draw2d.GetVertices()[startPos];
    }
}