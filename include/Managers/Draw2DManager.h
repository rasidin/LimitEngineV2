/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Draw2DManager.h
 @brief DrawManager Class (2D)
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#pragma once

#include <LERenderer>

#include "Core/Singleton.h"
#include "Renderer/Vertex.h"
#include "Renderer/VertexBuffer.h"

namespace LimitEngine {
    typedef Vertex<VERTEXTYPE(PCT)> Vertex2D;
    typedef VertexBuffer<VERTEXTYPE(PCT)> VertexBuffer2D;

    class Draw2DManager;
    typedef Singleton<Draw2DManager, LimitEngineMemoryCategory_Graphics> SingletonDraw2DManager;
    class Draw2DManager : public SingletonDraw2DManager
    {
        static constexpr uint32 Vertex2DReserved = 3 * 200;
    public:
        Draw2DManager();
        virtual ~Draw2DManager();

        void Init();
        void Term();

        void EndOfFrame();

        Vertex2D* GetVertexBuffer2D(uint32 size);                           //!< Get temporary vertex buffer for drawing GUI...

        void DrawScreen(Shader *Shader = nullptr, ConstantBuffer *buffer = nullptr);
        void FlushDraw2D(const RendererFlag::PrimitiveTypes &PrimitiveType, Shader *Shader = nullptr, ConstantBuffer *buffer = nullptr);
    private:
        VertexBuffer2D   mVertexbuffer_draw2d;     //!< Vertex buffer for drawing 2D
        VertexBuffer2D   mVertexbuffer_drawscr;    //!< Vertex buffer for drawing screen (ex.post filter)
    
        ShaderRefPtr     mShader_draw2d;           //!< Shader for drawing 2D
        TextureRefPtr    mNullTexture;             //!< Null texture for initializing

        uint32           mVertex_draw2d_used;      //!< Used size of vertex buffer for drawing 2D
        uint32           mVertex_draw2d_start;     //!< Start offset of vertex buffer for drawing 2D
    };
}
#define LE_Draw2DManager Draw2DManager::GetSingleton()
