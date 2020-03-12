/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  VertexBuffer.cpp
 @brief Vertex Buffer
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

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
#else
#error No implementation for VertexBuffer
#endif

namespace LimitEngine {
VertexBufferImpl* CreateImplementation() 
{
#ifdef USE_OPENGLES
    return new VertexBufferImpl_OpenGLES();
#elif defined(USE_DX9)
    return new VertexBufferImpl_DirectX9();
#elif defined(USE_DX11)
    return new VertexBufferImpl_DirectX11();
#else
#error No implementation for VertexBuffer New
#endif
}

}