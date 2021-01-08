/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file DrawCommand.h
@brief Post processor for Resolving final color
@author minseob
**********************************************************************/
#ifndef LIMITENGINEV2_RENDERER_DRAWCOMMAND_H_
#define LIMITENGINEV2_RENDERER_DRAWCOMMAND_H_

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
    static void BindVertexBuffer(VertexBufferGeneric* VertexBuffer);
    static void BindIndexBuffer(IndexBuffer* InIndexBuffer);
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
    static void UpdateConstantBuffer(ConstantBuffer* buffer, void* data, size_t size);
    static void SetConstantBuffer(uint32 index, ConstantBuffer* buffer);
    static void SetCulling(uint32 cull);
    static void SetEnable(uint32 f);
    static void SetDisable(uint32 f);
    static void SetBlendFunc(uint32 rt, RendererFlag::BlendFlags func);
    static void SetDepthFunc(RendererFlag::TestFlags f);
    static void CopyBuffer(void *Dst, uint32 DstOffset, void *Org, uint32 OrgOffset, uint32 Size);
    static void ResourceBarrier(class Texture *InTexture, const ResourceState& InResourceState);
    static void ResourceBarrier(class VertexBufferGeneric* InVertexBuffer, const ResourceState& InResourceState);
    static void ResourceBarrier(class IndexBuffer *InIndexBuffer, const ResourceState &InResourceState);
    static void SetMarker(const char *InMarkerName);
    static void BeginEvent(const char *InEventName);
    static void EndEvent();
}; // DrawCommand
}
#endif // LIMITENGINEV2_RENDERER_DRAWCOMMAND_H_