/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  CommandBuffer.h
@brief DrawCommand (just for creation command in commnadbuffer)
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include <LERenderer>

#include <LEFloatVector2.h>
#include <LEFloatVector4.h>

#include "Managers/RenderTargetPoolManager.h"

namespace LimitEngine {

class DrawCommand
{ public:
    // ===========================================
    // Commands
    // ===========================================
    static void ClearScreen(const LEMath::FloatColorRGBA &color);
    static void BeginScene();
    static void EndScene();
    static void BeginDrawing();
    static void EndDrawing();
    static void BindVertexBuffer(void *handle, void *buffer, uint32 offset, uint32 size, uint32 stride);
    static void BindIndexBuffer(void *handle);
	static void BindTargetTexture(uint32 index, Texture *texture);
    static void BindSampler(uint32 index, SamplerState *sampler);
    static void BindTexture(uint32 index, Texture *texture);
    static void BindTexture(uint32 index, const PooledRenderTarget &texture);
    static void BindTexture(uint32 index, const PooledDepthStencil &texture);
    static void BindShader(Shader *shader);
    static void BindShader(const char *name);
    static void BindConstantBuffer(ConstantBuffer *cb);
	static void Dispatch(int x, int y, int z);
    static void DrawPrimitive(RendererFlag::PrimitiveTypes type, uint32 offset, uint32 count);
    static void DrawIndexedPrimitive(RendererFlag::PrimitiveTypes type, uint32 vtxcount, uint32 count);
    static void SetRenderTarget(uint32 index, Texture *color, Texture *depthstencil, uint32 surfaceIndex = 0);
    static void SetFVF(uint32 fvf);
    static void SetShaderUniformFloat1(Shader *shader, ConstantBuffer *buffer, int location, const float v);
    static void SetShaderUniformFloat2(Shader *shader, ConstantBuffer *buffer, int location, const LEMath::FloatVector2 &value);
    static void SetShaderUniformFloat3(Shader *shader, ConstantBuffer *buffer, int location, const LEMath::FloatVector3 &value);
    static void SetShaderUniformFloat4(Shader *shader, ConstantBuffer *buffer, int location, const LEMath::FloatVector4 &v);
    static void SetShaderUniformInt1(Shader *shader, ConstantBuffer *buffer, int location, const int32 v);
    static void SetShaderUniformInt2(Shader *shader, ConstantBuffer *buffer, int location, const LEMath::IntVector2 &v);
    static void SetShaderUniformInt3(Shader *shader, ConstantBuffer *buffer, int location, const LEMath::IntVector3 &v);
    static void SetShaderUniformInt4(Shader *shader, ConstantBuffer *buffer, int location, const LEMath::IntVector4 &v);
    static void SetShaderUniformMatrix4(Shader *shader, ConstantBuffer *buffer, int location, int size, float *pointer);
    static void SetCulling(uint32 cull);
    static void SetEnable(uint32 f);
    static void SetDisable(uint32 f);
    static void SetBlendFunc(uint32 rt, RendererFlag::BlendFlags func);
    static void SetDepthFunc(RendererFlag::TestFlags f);
    static void SetMarker(const char *InMarkerName);
    static void BeginEvent(const char *InEventName);
    static void EndEvent();
}; // DrawCommand
}
