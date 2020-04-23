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

namespace LimitEngine {

class DrawCommand
{ public:
    // ===========================================
    // Commands
    // ===========================================
    static void ClearScreen(const LEMath::FloatColorRGBA &color);
    static void BeginScene();
    static void EndScene();
    static void Present();
    static void BeginDrawing();
    static void EndDrawing();
    static void BindVertexBuffer(void *handle, void *buffer, uint32 offset, uint32 size, uint32 stride);
    static void BindIndexBuffer(void *handle);
	static void BindTargetTexture(uint32 index, Texture *texture);
    static void BindTexture(uint32 index, Texture *texture);
    static void BindShader(Shader *shader);
    static void BindShader(const char *name);
	static void Dispatch(int x, int y, int z);
    static void DrawPrimitive(RendererFlag::PrimitiveTypes type, uint32 offset, uint32 count);
    static void DrawPrimitiveUp(RendererFlag::PrimitiveTypes type, uint32 count, void *data, uint32 stride);
    static void DrawIndexedPrimitive(RendererFlag::PrimitiveTypes type, uint32 vtxcount, uint32 count);
    static void SetRenderTarget(uint32 index, Texture *color, Texture *depthstencil, uint32 surfaceIndex = 0);
    static void SetFVF(uint32 fvf);
    static void SetShaderUniformFloat1(Shader *shader, int location, const float v);
    static void SetShaderUniformFloat2(Shader *shader, int location, const LEMath::FloatVector2 &value);
    static void SetShaderUniformFloat4(Shader *shader, int location, const LEMath::FloatVector4 &v);
    static void SetShaderUniformMatrix4(Shader *shader, int location, int size, float *pointer);
    static void SetCulling(uint32 cull);
    static void SetEnable(uint32 f);
    static void SetDisable(uint32 f);
    static void SetBlendFunc(uint32 rt, RendererFlag::BlendFlags func);
    static void SetDepthFunc(RendererFlag::TestFlags f);
}; // DrawCommand
}
