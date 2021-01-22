/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  VertexBuffer.cpp
 @brief Vertex Buffer
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/Vertex.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Shader.h"
#include "Managers/DrawManager.h"

#ifdef USE_OPENGLES
#include "OpenGLES/LE_VertexBufferImpl_OpenGLES.h"
#elif defined(USE_DX9)
#include "DirectX9/LE_VertexBufferImpl_DirectX9.h"
#elif defined(USE_DX11)
#include "../Platform/DirectX11/VertexBufferImpl_DirectX11.inl"
#elif defined(USE_DX12)
#include "../Platform/DirectX12/VertexBufferImpl_DirectX12.inl"
#else
#error No implementation for VertexBuffer
#endif

namespace LimitEngine {
const char* FVF_NAMES[FVF_INDEX_MAX] = {
    "NONE",
    "POSITION",
    "NORMAL",
    "COLOR",
    "TEXCOORD",
    "WEIGHT",
    "TANGENT",
    "BINORMAL"
};
const RendererFlag::BufferFormat FVF_FORMATS[FVF_INDEX_MAX] = {
    RendererFlag::BufferFormat::Unknown,
    RendererFlag::BufferFormat::R32G32B32_Float,
    RendererFlag::BufferFormat::R32G32B32_Float,
    RendererFlag::BufferFormat::R8G8B8A8_UNorm,
    RendererFlag::BufferFormat::R32G32_Float,
    RendererFlag::BufferFormat::R32_Float,
    RendererFlag::BufferFormat::R32G32B32_Float,
    RendererFlag::BufferFormat::R32G32B32_Float,
};
VertexBufferGeneric::VertexBufferGeneric()
{
#ifdef USE_OPENGLES
    mImpl = new VertexBufferImpl_OpenGLES();
#elif defined(USE_DX9)
    mImpl = new VertexBufferImpl_DirectX9();
#elif defined(USE_DX11)
    mImpl = new VertexBufferImpl_DirectX11();
#elif defined(USE_DX12)
    mImpl = new VertexBufferImpl_DirectX12();
#else
#error No implementation for VertexBuffer New
#endif
}
VertexBufferGeneric::~VertexBufferGeneric()
{
    if (mImpl) {
        delete mImpl;
        mImpl = nullptr;
    }
}
}