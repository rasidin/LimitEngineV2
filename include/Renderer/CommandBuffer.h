/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  CommandBuffer.h
@brief CommandBuffer for rendering
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include <LERenderer>
#include <LEManagers>

#include <LEFloatVector2.h>
#include <LEFloatVector4.h>
#include <LEIntVector4.h>

#include "Core/Singleton.h"
#include "Core/Mutex.h"
#include "Core/ReferenceCountedPointer.h"

#include "Managers/RenderTargetPoolManager.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/SamplerState.h"
#include "Renderer/PipelineState.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
class CommandImpl : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    CommandImpl() {}
    virtual ~CommandImpl() {}

    virtual void Init(void *Parameter) = 0;
    virtual void Term() = 0;

    virtual void* GetCommandListHandle() = 0;
    virtual void ReadyToExecute() = 0;
    virtual void Finalize(uint64 CompletedFenceValue) = 0;
    virtual void ProcessAfterPresent() = 0;

    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void SetViewport(const LEMath::IntRect &rect) = 0;
    virtual void SetScissorRect(const LEMath::IntRect& rect) = 0;
    virtual bool PrepareForDrawing() = 0;
    virtual bool PrepareForDrawingModel() = 0;
    virtual void ClearCaches() = 0;
    virtual void ClearScreen(const LEMath::FloatColorRGBA &Color) = 0;
    virtual void BindVertexBuffer(VertexBufferGeneric *vb) = 0;
    virtual void BindIndexBuffer(IndexBuffer *ib) = 0;
    virtual void SetConstantBuffer(uint32 index, ConstantBuffer *cb) = 0;
    virtual void SetPipelineState(PipelineState *pso) = 0;
    virtual void UpdateConstantBuffer(ConstantBuffer *cb, void *data, size_t size) = 0;
    virtual void BindTargetTexture(uint32 Index, Texture *Tex) = 0;
    virtual void BindSampler(uint32 Index, SamplerState *Sampler) = 0;
    virtual void BindTexture(uint32 Index, TextureInterface *Tex) = 0;
    virtual void Dispatch(int X, int Y, int Z) = 0;
    virtual void DrawPrimitive(uint32 Primitive, uint32 Offset, uint32 Count) = 0;
    virtual void DrawIndexedPrimitive(uint32 Primitive, uint32 VertexCount, uint32 Count) = 0;
    virtual void SetRenderTarget(uint32 Index, const TextureRendererAccessor &Color, const TextureRendererAccessor &Depth, uint32 SurfaceIndex) = 0;
    virtual void CopyResource(void* Dst, uint32 DstOffset, void* Org, uint32 OrgOffset, uint32 Size) = 0;
    virtual void ResourceBarrier(void* Resource, const ResourceState& Before, const ResourceState& After) = 0;
    virtual void SetMarker(const char *InMarkerName) = 0;
    virtual void BeginEvent(const char *InEventName) = 0;
    virtual void EndEvent() = 0;
};
class CommandBuffer : public Object<LimitEngineMemoryCategory::Graphics>
{
    friend DrawManager;
    friend DrawCommand;
private:            // Private Definitions
    static const uint32 CommandReservedMemorySize = 1 * (1 << 20); // 1MB

private:            // Private Structure
                    // Render command list
    typedef struct _COMMAND_COMMON
    {
        enum Commands
        {
            cBeginScene = 0,
            cEndScene,
            cPresent,
            cBeginDrawing,
            cEndDrawing,
            cSetViewport,
            cSetScissorRect,
            cClearScreen,
            cBindSampler,
            cBindTexture,
            cBindTargetTexture,
            cBindPooledRenderTarget,
            cBindPooledDepthStencil,
            cBindVertexBuffer,
            cBindIndexBuffer,
            cDispatch,
            cDrawPrimitive,
            cDrawIndexedPrimitive,
            cSetPipelineState,
            cUpdateConstantBuffer,
            cSetConstantBuffer,
            cSetRenderTarget,
            cResourceBarrier,
            cSetMarker,
            cBeginEvent,
            cEndEvent,
            CommandNum,
        };
        uint16       commandType;               //!<Type of command                  [ 2 ]
        uint16       nextOffset;                // Offset to next command            [ 2 ]

        _COMMAND_COMMON() : nextOffset(1) {}
        _COMMAND_COMMON(uint16 t) : commandType(t), nextOffset(1) {}
        _COMMAND_COMMON(uint16 t, uint16 o) : commandType(t), nextOffset(o) {}

        virtual size_t GetSize() const { return 64u; } // Get Command Size
    } COMMAND_COMMON;    // [ 4 ]

    // Empty command
    typedef struct _COMMAND : public _COMMAND_COMMON
    {
        char                data[44];        //!<Data of command                    [ 44 ]
    } COMMAND;    // [ 64 | 64 ]
    // Begin scene
    typedef struct _COMMAND_BEGINSCENE : public _COMMAND_COMMON
    {
        _COMMAND_BEGINSCENE() : _COMMAND_COMMON(cBeginScene) {}
    } COMMAND_BEGINSCENE;
    // End scene
    typedef struct _COMMAND_ENDSCENE : public _COMMAND_COMMON
    {
        _COMMAND_ENDSCENE() : _COMMAND_COMMON(cEndScene) {}
    } COMMAND_ENDSCENE;
    // Begin drawing
    typedef struct _COMMAND_BEGINDRAWING : public _COMMAND_COMMON
    {
        _COMMAND_BEGINDRAWING() : _COMMAND_COMMON(cBeginDrawing) {}
    } COMMAND_BEGINDRAWING;
    // End drawing
    typedef struct _COMMAND_ENDDRAWING : public _COMMAND_COMMON
    {
        _COMMAND_ENDDRAWING() : _COMMAND_COMMON(cEndDrawing) {}
    } COMMAND_ENDDRAWING;
    typedef struct _COMMAND_SETVIEWPORT : public _COMMAND_COMMON
    {
        _COMMAND_SETVIEWPORT(const LEMath::IntRect& rect) : _COMMAND_COMMON(cSetViewport), vprect(rect) {}
        LEMath::IntRect vprect;
    } COMMAND_SETVIEWPORT;
    typedef struct _COMMAND_SETSCISSORRECT : public _COMMAND_COMMON
    {
        _COMMAND_SETSCISSORRECT(const LEMath::IntRect& rect) : _COMMAND_COMMON(cSetScissorRect), screct(rect) {}
        LEMath::IntRect screct;
    } COMMAND_SETSCISSORRECT;
    // Clear screen
    typedef struct _COMMAND_CLEARSCREEN : public _COMMAND_COMMON
    {
        _COMMAND_CLEARSCREEN(const LEMath::FloatColorRGBA &c)
            : _COMMAND_COMMON(cClearScreen)
            , color(c)
        {
        }
        LEMath::FloatColorRGBA       color;
    } COMMAND_CLEARSCREEN;
    // Bind vertex buffer before drawing model
    typedef struct _COMMAND_BINDVERTEXBUFFER : public _COMMAND_COMMON
    {
        _COMMAND_BINDVERTEXBUFFER(VertexBufferGeneric* vb)
            : _COMMAND_COMMON(cBindVertexBuffer)
            , vertexbuffer(vb)
        {
            vb->AddReferenceCounter();
        }
        VertexBufferGeneric* vertexbuffer;
    } COMMAND_BINDVERTEXBUFFER;
    // Bind index buffer before drawing model
    typedef struct _COMMAND_BINDINDEXBUFFER : public _COMMAND_COMMON
    {
        _COMMAND_BINDINDEXBUFFER(IndexBuffer *ib)
            : _COMMAND_COMMON(cBindIndexBuffer)
            , indexbuffer(ib)
        {
            ib->AddReferenceCounter();
        }
        IndexBuffer* indexbuffer;
    } COMMAND_BINDINDEXBUFFER;
    // Bind target(RW) texture
    typedef struct _COMMAND_BINDTARGETTEXTURE : public _COMMAND_COMMON
    {
        _COMMAND_BINDTARGETTEXTURE(uint32 i, Texture *t)
            : _COMMAND_COMMON(cBindTargetTexture)
            , index(i)
            , texture(t)
        {}
        uint32          index;
        Texture        *texture;
    } COMMAND_BINDTARGETTEXTURE;
    // Bind texture sampler
    typedef struct _COMMAND_BINDSAMPLER : public _COMMAND_COMMON
    {
        _COMMAND_BINDSAMPLER(uint32 n, SamplerState *s)
            : _COMMAND_COMMON(cBindSampler)
            , index(n)
            , sampler(s)
        {
            sampler->AddReferenceCounter();
        }
        uint32              index;
        SamplerState       *sampler;
    } COMMAND_BINDSAMPLER;
    // Bind texture before drawing model
    typedef struct _COMMAND_BINDTEXTURE : public _COMMAND_COMMON
    {
        _COMMAND_BINDTEXTURE(uint32 i, TextureInterface *t)
            : _COMMAND_COMMON(cBindTexture)
            , index(i)
            , texture(t)
        {
            t->AddReferenceCounter();
        }
        uint32               index;
        TextureInterface    *texture;
    } COMMAND_BINDTEXTURE;
    typedef struct _COMMAND_BINDPOOLEDRENDERTARGET : public _COMMAND_COMMON
    {
        _COMMAND_BINDPOOLEDRENDERTARGET(uint32 i, const PooledRenderTarget &t)
            : _COMMAND_COMMON(cBindPooledRenderTarget)
            , index(i)
            , texture(t)
        {}
        uint32             index;
        PooledRenderTarget texture;
    } COMMAND_BINDPOOLEDRENDERTARGET;
    typedef struct _COMMAND_BINDPOOLEDDEPTHSTENCIL : public _COMMAND_COMMON
    {
        _COMMAND_BINDPOOLEDDEPTHSTENCIL(uint32 i, const PooledDepthStencil &t)
            : _COMMAND_COMMON(cBindPooledDepthStencil)
            , index(i)
            , texture(t)
        {}
        uint32             index;
        PooledDepthStencil texture;
    } COMMAND_BINDPOOLEDDEPTHSTENCIL;
    // Set pipelinestate
    typedef struct _COMMAND_SETPIPELINESTATE : public _COMMAND_COMMON
    {
        _COMMAND_SETPIPELINESTATE(PipelineState *p)
            : _COMMAND_COMMON(cSetPipelineState)
            , pipelinestate(p)
        {
            p->AddReferenceCounter();
        }
        PipelineState *pipelinestate;
    } COMMAND_SETPIPELINESTATE;
    // Upload constant buffer using local cpu memory
    typedef struct _COMMAND_UPDATECONSTANTBUFFER : public _COMMAND_COMMON
    {
        _COMMAND_UPDATECONSTANTBUFFER(ConstantBuffer* cb, void* dt, size_t sz)
            : _COMMAND_COMMON(cUpdateConstantBuffer)
            , buffer(cb)
            , data(dt)
            , size(sz)
        {
            cb->AddReferenceCounter();
        }
        ConstantBuffer* buffer;
        void* data;
        size_t size;
    } COMMAND_UPDATECONSTANTBUFFER;
    // Set constant buffer
    typedef struct _COMMAND_SETCONSTANTBUFFER : public _COMMAND_COMMON
    {
        _COMMAND_SETCONSTANTBUFFER(uint32 idx, ConstantBuffer* cb)
            : _COMMAND_COMMON(cSetConstantBuffer)
            , index(idx)
            , buffer(cb)
        {
            cb->AddReferenceCounter();
        }
        uint32 index;
        ConstantBuffer* buffer;
    } COMMAND_SETCONSTANTBUFFER;
    // Dispatch
    typedef struct _COMMAND_DISPATCH : public _COMMAND_COMMON
    {
        _COMMAND_DISPATCH(int x, int y, int z)
            : _COMMAND_COMMON(cDispatch)
            , gridx(x)
            , gridy(y)
            , gridz(z)
        {}
        int gridx;
        int gridy;
        int gridz;
    } COMMAND_DISPATCH;
    // Draw primitive
    typedef struct _COMMAND_DRAWPRIMITIVE : public _COMMAND_COMMON
    {
        _COMMAND_DRAWPRIMITIVE(RendererFlag::PrimitiveTypes t, uint32 o, uint32 c)
            : _COMMAND_COMMON(cDrawPrimitive)
            , primitive(static_cast<uint32>(t))
            , offset(o)
            , count(c)
        {}
        uint32              primitive;
        uint32              offset;
        uint32              count;
    } COMMAND_DRAWPRIMITIVE;
    // Draw indexed primitive
    typedef struct _COMMAND_DRAWINDEXEDPRIMITIVE : public _COMMAND_COMMON
    {
        _COMMAND_DRAWINDEXEDPRIMITIVE(RendererFlag::PrimitiveTypes t, uint32 v, uint32 c)
            : _COMMAND_COMMON(cDrawIndexedPrimitive)
            , primitive(static_cast<uint32>(t))
            , vtxcount(v)
            , count(c)
        {}
        uint32                 primitive;
        uint32               vtxcount;
        uint32                 count;
    } COMMAND_DRAWINDEXEDPRIMITIVE;
    // Set render target for drawing
    typedef struct _COMMAND_SETRENDERTARGET : public _COMMAND_COMMON
    {
        _COMMAND_SETRENDERTARGET(uint32 idx, TextureInterface *c, TextureInterface *d, uint32 suridx)
            : _COMMAND_COMMON(cSetRenderTarget)
            , index(idx)
            , surfaceIndex(suridx)
            , color(c)
            , depthstencil(d)
        {
            if (color)
                color->AddReferenceCounter();
            if (depthstencil)
                depthstencil->AddReferenceCounter();
        }
        uint32                  index;
        uint32                  surfaceIndex;
        TextureInterface       *color;
        TextureInterface       *depthstencil;
    } COMMAND_SETRENDERTARGET;
    typedef struct _COMMAND_RESOURCEBARRIER : public _COMMAND_COMMON
    {
        _COMMAND_RESOURCEBARRIER(TextureInterface* InTexture, const ResourceState& InState)
            : _COMMAND_COMMON(cResourceBarrier)
            , texture(InTexture)
            , type(Type::Texture)
            , state(InState)
        {
            InTexture->AddReferenceCounter();
        }
        _COMMAND_RESOURCEBARRIER(IndexBuffer* InIndexBuffer, const ResourceState& InState)
            : _COMMAND_COMMON(cResourceBarrier)
            , indexBuffer(InIndexBuffer)
            , type(Type::IndexBuffer)
            , state(InState)
        {
            InIndexBuffer->AddReferenceCounter();
        }
        _COMMAND_RESOURCEBARRIER(VertexBufferGeneric* InVertexBuffer, const ResourceState& InState)
            : _COMMAND_COMMON(cResourceBarrier)
            , vertexBuffer(InVertexBuffer)
            , type(Type::VertexBuffer)
            , state(InState)
        {
            InVertexBuffer->AddReferenceCounter();
        }
        union {
            TextureInterface        *texture;
            IndexBuffer             *indexBuffer;
            VertexBufferGeneric     *vertexBuffer;
        };
        enum class Type : uint8 {
            Texture = 0,
            IndexBuffer,
            VertexBuffer
        } type;
        ResourceState        state;
    } COMMAND_RESOURCEBARRIER;
    typedef struct _COMMAND_SETMARKER : public _COMMAND_COMMON
    {
        _COMMAND_SETMARKER(char *InMarkerName)
            : _COMMAND_COMMON(cSetMarker)
            , MarkerName(InMarkerName)
        {}
        char                *MarkerName;
    } COMMAND_SETMARKER;
    typedef struct _COMMAND_BEGINEVENT : public _COMMAND_COMMON
    {
        _COMMAND_BEGINEVENT(char *InEventName)
            : _COMMAND_COMMON(cBeginEvent)
            , EventName(InEventName)
        {}
        char               *EventName;
    } COMMAND_BEGINEVENT;
    typedef struct _COMMAND_ENDEVENT : public _COMMAND_COMMON
    {
        _COMMAND_ENDEVENT()
            : _COMMAND_COMMON(cEndEvent)
        {}
    } COMMAND_ENDEVENT;
    typedef struct _COMMAND_PRESENT : public _COMMAND_COMMON
    {
        _COMMAND_PRESENT()
            : _COMMAND_COMMON(cPresent)
        {}
    } COMMAND_PRESENT;
public:
    // Ctor & Dtor
    CommandBuffer(size_t bufferSize = CommandReservedMemorySize);
    virtual ~CommandBuffer();

    void Init(void *Parameter) { if (mImpl) mImpl->Init(Parameter); }
    void Term() { if (mImpl) mImpl->Term(); }
    void ReleaseRendererResources();

    void* GetCommandListHandle() const { return mImpl->GetCommandListHandle(); }
    
    void ReadyToExecute() const { mImpl->ReadyToExecute(); }
    void Finalize(uint64 CompletedFenceIndex) const { mImpl->Finalize(CompletedFenceIndex); }
    void ProcessAfterPresent() const { mImpl->ProcessAfterPresent(); }

    uint32 CalculateCommandOffset(COMMAND *cmd);

    void SetCommand(COMMAND_COMMON *cmd);
    void AddCommand(COMMAND_COMMON *cmd);

    void LockWriteCommandBuffer() { mMutex.Lock(); }
    void Flush(RenderState *rs);
private:
    void* allocateFromCommandBuffer(size_t size);
    void* copyDataToGPUBuffer(void *data, size_t size);
    void* duplicateBuffer(void* data, size_t size);
    float* copyMatrixToBuffer(float *ptr);
    char* duplicateString(const char *src);

    COMMAND* popPushCommandBuffer();
    COMMAND* nextPushCommandBuffer(int count = 1);
    COMMAND* nextPullCommandBuffer();
private:
    CommandImpl             *mImpl;

    void                    *mCommandBuffer;
    void                    *mPushCommandBufferPointer;
    void                    *mPullCommandBufferPointer;

    struct {
        void Clear() {
            Textures.Clear();
            IndexBuffers.Clear();
            VertexBuffers.Clear();
            ConstantBuffers.Clear();
            SamplerStates.Clear();
            PipelineStates.Clear();
        }

        VectorArray<TextureInterface*>      Textures;
        VectorArray<IndexBuffer*>           IndexBuffers;
        VectorArray<VertexBufferGeneric*>   VertexBuffers;
        VectorArray<ConstantBuffer*>        ConstantBuffers;
        VectorArray<SamplerState*>          SamplerStates;
        VectorArray<PipelineState*>         PipelineStates;

        void Add(TextureInterface* t)       { if (Textures.IndexOf(t) < 0) Textures.Add(t); }
        void Add(IndexBuffer* i)            { if (IndexBuffers.IndexOf(i) < 0) IndexBuffers.Add(i); }
        void Add(VertexBufferGeneric* v)    { if (VertexBuffers.IndexOf(v) < 0) VertexBuffers.Add(v); }
        void Add(ConstantBuffer* c)         { if (ConstantBuffers.IndexOf(c) < 0) ConstantBuffers.Add(c); }
        void Add(SamplerState* s)           { if (SamplerStates.IndexOf(s) < 0) SamplerStates.Add(s); }
        void Add(PipelineState *p)          { if (PipelineStates.IndexOf(p) < 0) PipelineStates.Add(p); }
    } ReservedRendererResources;

    Mutex                    mMutex;
};
}
