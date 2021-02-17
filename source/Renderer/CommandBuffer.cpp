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
@file  CommandBuffer.h
@brief CommandBuffer for rendering
@author minseob (https://github.com/rasidin)
**********************************************************************/
#include "Renderer/CommandBuffer.h"

#include <LEFloatMatrix4x4.h>

#include "Core/Debug.h"
#include "Core/Util.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/PipelineStateDescriptor.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/RenderContext.h"
#include "Renderer/VertexBuffer.h"
#include "Managers/ShaderManager.h"
#include "Managers/DrawManager.h"

#ifdef USE_DX11
#include "../Platform/DirectX11/CommandImpl_DirectX11.inl"
#elif USE_DX12
#include "../Platform/DirectX12/CommandImpl_DirectX12.inl"
#else
#error No implementation for CommandBuffer
#endif

namespace LimitEngine {
CommandBuffer::CommandBuffer(size_t bufferSize)
    : mCommandBuffer(NULL)
    , mImpl(nullptr)
    , mPushCommandBufferPointer(NULL)
    , mPullCommandBufferPointer(NULL)
    , mMutex()
{
#ifdef USE_DX11
    mImpl = new CommandImpl_DirectX11();
#elif USE_DX12
    mImpl = new CommandImpl_DirectX12();
#endif
    mCommandBuffer = malloc(bufferSize);
    mPushCommandBufferPointer = mCommandBuffer;
    mPullCommandBufferPointer = mCommandBuffer;
}
CommandBuffer::~CommandBuffer()
{
    if (mImpl) {
        delete mImpl;
        mImpl = nullptr;
    }
    if (mCommandBuffer) {
        free(mCommandBuffer);
        mCommandBuffer = NULL;
    }
}

void CommandBuffer::ReleaseRendererResources()
{
    if (ReservedRendererResources.Textures.count())
    for (TextureInterface* texture : ReservedRendererResources.Textures)
        delete texture;
    if (ReservedRendererResources.IndexBuffers.count())
    for (IndexBuffer* indexbuffer : ReservedRendererResources.IndexBuffers)
        delete indexbuffer;
    if (ReservedRendererResources.VertexBuffers.count())
    for (VertexBufferGeneric* vertexbuffer : ReservedRendererResources.VertexBuffers)
        delete vertexbuffer;
    if (ReservedRendererResources.ConstantBuffers.count())
    for (ConstantBuffer* buffer : ReservedRendererResources.ConstantBuffers)
        delete buffer;
    if (ReservedRendererResources.SamplerStates.count())
    for (SamplerState* samplerstate : ReservedRendererResources.SamplerStates)
        delete samplerstate;
    if (ReservedRendererResources.PipelineStates.count())
    for (PipelineState* pipelinestate : ReservedRendererResources.PipelineStates)
        delete pipelinestate;
    ReservedRendererResources.Clear();
}

CommandBuffer::COMMAND* CommandBuffer::popPushCommandBuffer()
{
    Mutex::ScopedLock scopedLock(mMutex);
    COMMAND *currentCommandBufferPointer = (COMMAND*)mPushCommandBufferPointer;
    mPushCommandBufferPointer = (COMMAND*)mPushCommandBufferPointer + 1;
    if ((uintptr_t)mPushCommandBufferPointer >= (uintptr_t)mCommandBuffer + CommandReservedMemorySize)
        mPushCommandBufferPointer = mCommandBuffer;
    return (COMMAND*)currentCommandBufferPointer;
}

CommandBuffer::COMMAND* CommandBuffer::nextPushCommandBuffer(int count)
{
    Mutex::ScopedLock scopedLock(mMutex);
    void *LastPushCommand = mPushCommandBufferPointer;
    mPushCommandBufferPointer = (COMMAND*)mPushCommandBufferPointer + count;
    if (((intptr_t)mPushCommandBufferPointer) - ((intptr_t)mCommandBuffer) >= CommandReservedMemorySize) {
        intptr_t overMemoryOffset = GetSizeAlign<intptr_t>((intptr_t)mPushCommandBufferPointer - ((intptr_t)mCommandBuffer + CommandReservedMemorySize), static_cast<intptr_t>(sizeof(COMMAND)));
        mPushCommandBufferPointer = (uint8*)mCommandBuffer + overMemoryOffset;
    }
    return (COMMAND*)mPushCommandBufferPointer;
}

CommandBuffer::COMMAND* CommandBuffer::nextPullCommandBuffer()
{
    COMMAND *currentCommand = (COMMAND*)mPullCommandBufferPointer;
    COMMAND *nextCommand = currentCommand + currentCommand->nextOffset;
    if ((uintptr_t)mCommandBuffer + CommandReservedMemorySize <= (uintptr_t)nextCommand) {
        intptr_t leftCountCB = ((intptr_t)mCommandBuffer + CommandReservedMemorySize - (intptr_t)currentCommand) / sizeof(COMMAND);
        nextCommand = (COMMAND*)mCommandBuffer + currentCommand->nextOffset - leftCountCB;
    }
    mPullCommandBufferPointer = nextCommand;
    return (COMMAND*)mPullCommandBufferPointer;
}

void CommandBuffer::SetCommand(COMMAND_COMMON *cmd)
{
    ::memcpy(mPushCommandBufferPointer, cmd, sizeof(COMMAND));
}

void CommandBuffer::AddCommand(CommandBuffer::COMMAND_COMMON *cmd)
{
    Mutex::ScopedLock scopedLock(mMutex);
    ::memcpy(mPushCommandBufferPointer, cmd, sizeof(COMMAND));
    nextPushCommandBuffer();
}

void* CommandBuffer::allocateFromCommandBuffer(size_t size)
{
    Mutex::ScopedLock scopedLock(mMutex);
    size_t alignedSize = GetSizeAlign(size, sizeof(COMMAND));
    size_t countCB = alignedSize / sizeof(COMMAND);
    void *outputPointer = mPushCommandBufferPointer;
    // Allocate from ring buffer
    if (uintptr_t(mPushCommandBufferPointer) + alignedSize > uintptr_t(mCommandBuffer) + CommandReservedMemorySize) {
        size_t leftCountCB = (intptr_t(mCommandBuffer) + CommandReservedMemorySize - intptr_t(mPushCommandBufferPointer)) / sizeof(COMMAND);
        outputPointer = mCommandBuffer;
        nextPushCommandBuffer(static_cast<int>(countCB + leftCountCB));
    }
    else
    {
        nextPushCommandBuffer(static_cast<int>(countCB));
    }
    
    return outputPointer;
}

void* CommandBuffer::duplicateBuffer(void* data, size_t size)
{
    void* output = allocateFromCommandBuffer(size);
    ::memcpy(output, data, size);
    return output;
}

float* CommandBuffer::copyMatrixToBuffer(float *ptr)
{
    float *output = static_cast<float*>(allocateFromCommandBuffer(sizeof(LEMath::FloatMatrix4x4)));
    ::memcpy(output, ptr, sizeof(LEMath::FloatMatrix4x4));
    return output;
}

char* CommandBuffer::duplicateString(const char *src)
{
    size_t srclength = strlen(src) + 1;
    char *output = static_cast<char*>(allocateFromCommandBuffer(srclength));
    ::memcpy(output, src, srclength);
    return output;
}

uint32 CommandBuffer::CalculateCommandOffset(COMMAND *cmd)
{
    if ((intptr_t)mPushCommandBufferPointer < (intptr_t)cmd) { // Back to origin
        size_t leftCountCB = (intptr_t(mCommandBuffer) + CommandReservedMemorySize - intptr_t(cmd)) / sizeof(COMMAND);
        size_t countFromOrigin = (intptr_t(mPushCommandBufferPointer) - intptr_t(mCommandBuffer)) / sizeof(COMMAND);
        return static_cast<uint32>(leftCountCB + countFromOrigin);
    }
    else {
        intptr_t nextOffset = (intptr_t(mPushCommandBufferPointer) - intptr_t(cmd)) / sizeof(COMMAND);
        return static_cast<uint32>(nextOffset);
    }
}

void CommandBuffer::Flush(RenderState *rs)
{
    // Locked by drawmanager....
    void  *flushCommandBufferPointer = mPushCommandBufferPointer;
    mMutex.Unlock();

    uint32 cmdCount = 0u;
	Texture *nullTexture = RenderContext::GetSingleton().GetDefaultTexture(RenderContext::DefaultTextureTypeWhite);
    //gDebug.Print(">> Run command : %x to %x (%d)\n", (uintptr_t)mPullCommandBufferPointer, (uintptr_t)flushCommandBufferPointer, ((intptr_t)flushCommandBufferPointer - (intptr_t)mPullCommandBufferPointer) / sizeof(COMMAND));
    while (flushCommandBufferPointer != mPullCommandBufferPointer)
    {
        COMMAND *currentCommand = static_cast<COMMAND*>(mPullCommandBufferPointer);
        nextPullCommandBuffer();

        //gDebug.Print("Command[%03d] : %d\n", cmdCount++, currentCommand->commandType);

        switch (currentCommand->commandType)
        {
            case COMMAND::cBeginScene:
            {
                mImpl->BeginScene();
                rs->SetSceneBegan(true);
            } break;
            case COMMAND::cEndScene:
            {
                mImpl->EndScene();
                rs->SetSceneBegan(false);
            } break;
            case COMMAND::cBeginDrawing:
            {
                mImpl->PrepareForDrawingModel();
                rs->SetModelDrawingBegan(true);
            } break;
            case COMMAND::cEndDrawing:
            {
                rs->SetModelDrawingBegan(false);
            } break;
            case COMMAND::cSetViewport:
            {
                COMMAND_SETVIEWPORT* command = reinterpret_cast<COMMAND_SETVIEWPORT*>(currentCommand);
                mImpl->SetViewport(command->vprect);
            } break;
            case COMMAND::cSetScissorRect:
            {
                COMMAND_SETSCISSORRECT* command = reinterpret_cast<COMMAND_SETSCISSORRECT*>(currentCommand);
                mImpl->SetScissorRect(command->screct);
            } break;
            case COMMAND::cClearScreen:
            {
                COMMAND_CLEARSCREEN *command = reinterpret_cast<COMMAND_CLEARSCREEN*>(currentCommand);
                LEMath::FloatColorRGBA color = command->color;
                mImpl->ClearScreen(color);
            } break;
            case COMMAND::cBindVertexBuffer:
            {
                COMMAND_BINDVERTEXBUFFER *command = reinterpret_cast<COMMAND_BINDVERTEXBUFFER*>(currentCommand);
                mImpl->BindVertexBuffer(command->vertexbuffer);
                if (command->vertexbuffer->SubReferenceCounter() == 0)
                    ReservedRendererResources.Add(command->vertexbuffer);
            } break;
            case COMMAND::cBindIndexBuffer:
            {
                COMMAND_BINDINDEXBUFFER *command = reinterpret_cast<COMMAND_BINDINDEXBUFFER*>(currentCommand);
                mImpl->BindIndexBuffer(command->indexbuffer);
                if (command->indexbuffer->SubReferenceCounter() == 0)
                    ReservedRendererResources.Add(command->indexbuffer);
			} break;
			case COMMAND::cDispatch:
			{
				COMMAND_DISPATCH *command = reinterpret_cast<COMMAND_DISPATCH*>(currentCommand);
                mImpl->Dispatch(command->gridx, command->gridy, command->gridz);
			} break;
            case COMMAND::cDrawPrimitive:
            {
                COMMAND_DRAWPRIMITIVE *command = reinterpret_cast<COMMAND_DRAWPRIMITIVE*>(currentCommand);
                if (mImpl->PrepareForDrawing())
                    mImpl->DrawPrimitive(command->primitive, command->offset, command->count);
                mImpl->ClearCaches();
            } break;
            case COMMAND::cDrawIndexedPrimitive:
            {
                COMMAND_DRAWINDEXEDPRIMITIVE *command = reinterpret_cast<COMMAND_DRAWINDEXEDPRIMITIVE*>(currentCommand);
				if (mImpl->PrepareForDrawing())
                    mImpl->DrawIndexedPrimitive(command->primitive, command->vtxcount, command->count);
                mImpl->ClearCaches();
            } break;
            case COMMAND::cSetRenderTarget:
            {
                COMMAND_SETRENDERTARGET *command = reinterpret_cast<COMMAND_SETRENDERTARGET*>(currentCommand);
                mImpl->SetRenderTarget(command->index, command->color, command->depthstencil, command->surfaceIndex);
                if (command->color && command->color->SubReferenceCounter() == 0) {
                    ReservedRendererResources.Add(command->color);
                }
                if (command->depthstencil && command->depthstencil->SubReferenceCounter() == 0) {
                    ReservedRendererResources.Add(command->depthstencil);
                }
            } break;
			case COMMAND::cBindTargetTexture:
			{
				COMMAND_BINDTARGETTEXTURE *command = reinterpret_cast<COMMAND_BINDTARGETTEXTURE*>(currentCommand);
                mImpl->BindTargetTexture(command->index, command->texture);
                if (command->texture->SubReferenceCounter() == 0)
                    ReservedRendererResources.Add(command->texture);
			} break;
            case COMMAND::cBindSampler:
            {
                COMMAND_BINDSAMPLER *command = reinterpret_cast<COMMAND_BINDSAMPLER*>(currentCommand);
                if (command->sampler) {
                    mImpl->BindSampler(command->index, command->sampler);
                    if (command->sampler->SubReferenceCounter() == 0)
                        ReservedRendererResources.Add(command->sampler);
                }
            } break;
            case COMMAND::cBindTexture:
            {
                COMMAND_BINDTEXTURE *command = reinterpret_cast<COMMAND_BINDTEXTURE*>(currentCommand);
                if (command->texture) {
                    mImpl->BindTexture(command->index, command->texture);
                    if (command->texture->SubReferenceCounter() == 0)
                        ReservedRendererResources.Add(command->texture);
                } 
                else
                    mImpl->BindTexture(command->index, nullTexture);
            } break;
            case COMMAND::cBindPooledRenderTarget:
            {
                COMMAND_BINDPOOLEDRENDERTARGET *command = reinterpret_cast<COMMAND_BINDPOOLEDRENDERTARGET*>(currentCommand);
                if (command->texture.Get()) {
                    mImpl->BindTexture(command->index, command->texture.Get());
                    command->texture.Release();
                }
            } break;
            case COMMAND::cBindPooledDepthStencil:
            {
                COMMAND_BINDPOOLEDDEPTHSTENCIL *command = reinterpret_cast<COMMAND_BINDPOOLEDDEPTHSTENCIL*>(currentCommand);
                if (command->texture.Get()) {
                    mImpl->BindTexture(command->index, command->texture.Get());
                    command->texture.Release();
                }
            } break;
            case COMMAND::cSetPipelineState:
            {
                COMMAND_SETPIPELINESTATE* command = reinterpret_cast<COMMAND_SETPIPELINESTATE*>(currentCommand);
                if (command->pipelinestate) {
                    mImpl->SetPipelineState(command->pipelinestate);
                    if (command->pipelinestate->SubReferenceCounter() == 0)
                        ReservedRendererResources.Add(command->pipelinestate);
                }
            } break;
            case COMMAND::cUpdateConstantBuffer:
            {
                COMMAND_UPDATECONSTANTBUFFER* command = reinterpret_cast<COMMAND_UPDATECONSTANTBUFFER*>(currentCommand);
                if (command->buffer) {
                    mImpl->UpdateConstantBuffer(command->buffer, command->data, command->size);
                    if (command->buffer->SubReferenceCounter() == 0)
                        ReservedRendererResources.Add(command->buffer);
                }
            } break;
            case COMMAND::cSetConstantBuffer:
            {
                COMMAND_SETCONSTANTBUFFER* command = reinterpret_cast<COMMAND_SETCONSTANTBUFFER*>(currentCommand);
                mImpl->SetConstantBuffer(command->index, command->buffer);
                if (command->buffer->SubReferenceCounter() == 0)
                    ReservedRendererResources.Add(command->buffer);
            } break;
            case COMMAND::cResourceBarrier:
            {
                COMMAND_RESOURCEBARRIER* command = reinterpret_cast<COMMAND_RESOURCEBARRIER*>(currentCommand);
                void* resource = nullptr;
                ResourceState beforeState = ResourceState::Common;
                switch (command->type) {
                case COMMAND_RESOURCEBARRIER::Type::Texture:
                    resource = TextureRendererAccessor(command->texture).GetResource();
                    beforeState = TextureRendererAccessor(command->texture).GetResourceState();
                    break;
                case COMMAND_RESOURCEBARRIER::Type::IndexBuffer:
                    resource = IndexBufferRendererAccessor(command->indexBuffer).GetResource();
                    beforeState = IndexBufferRendererAccessor(command->indexBuffer).GetResourceState();
                    break;
                case COMMAND_RESOURCEBARRIER::Type::VertexBuffer:
                    resource = VertexBufferRendererAccessor(command->vertexBuffer).GetResource();
                    beforeState = VertexBufferRendererAccessor(command->vertexBuffer).GetResourceState();
                    break;
                }
                if (resource && beforeState != command->state)
                    mImpl->ResourceBarrier(resource, beforeState, command->state);
                switch (command->type) {
                case COMMAND_RESOURCEBARRIER::Type::Texture:
                    TextureRendererAccessor(command->texture).SetResourceState(command->state);
                    if (command->texture->SubReferenceCounter() == 0) {
                        ReservedRendererResources.Add(command->texture);
                    }
                    break;
                case COMMAND_RESOURCEBARRIER::Type::IndexBuffer:
                    IndexBufferRendererAccessor(command->indexBuffer).SetResourceState(command->state);
                    if (command->indexBuffer->SubReferenceCounter() == 0) {
                        ReservedRendererResources.Add(command->indexBuffer);
                    }
                    break;
                case COMMAND_RESOURCEBARRIER::Type::VertexBuffer:
                    VertexBufferRendererAccessor(command->vertexBuffer).SetResourceState(command->state);
                    if (command->vertexBuffer->SubReferenceCounter() == 0) {
                        ReservedRendererResources.Add(command->vertexBuffer);
                    }
                    break;
                }
            } break;
            case COMMAND::cSetMarker:
            {
                COMMAND_SETMARKER *command = reinterpret_cast<COMMAND_SETMARKER*>(currentCommand);
                if (command->MarkerName) {
                    mImpl->SetMarker(command->MarkerName);
                }
            } break;
            case COMMAND::cBeginEvent:
            {
                COMMAND_BEGINEVENT *command = reinterpret_cast<COMMAND_BEGINEVENT*>(currentCommand);
                if (command->EventName) {
                    mImpl->BeginEvent(command->EventName);
                }
            } break;
            case COMMAND::cEndEvent:
            {
                mImpl->EndEvent();
            } break;
            case COMMAND::cPresent:
            {
                LE_DrawManagerRendererAccessor.Finalize(this);
                LE_DrawManagerRendererAccessor.Present();
                mImpl->ProcessAfterPresent();
            } break;
            default:
                DEBUG_MESSAGE("[DrawManager] Unknown Command : %d\n", currentCommand->commandType);
                break;
        }
    }
}

// =============================================
// Command interface in DrawManager
#define COMMANDBUFFER_NEW new(LE_DrawManagerRendererAccessor.GetCommandBuffer()->popPushCommandBuffer())

void DrawCommand::ClearScreen(const LEMath::FloatColorRGBA &color)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_CLEARSCREEN(color);
}

void DrawCommand::BeginScene()
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BEGINSCENE();
}

void DrawCommand::EndScene()
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_ENDSCENE();
}

void DrawCommand::BeginDrawing()
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BEGINDRAWING();
}

void DrawCommand::EndDrawing()
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_ENDDRAWING();
}

void DrawCommand::SetViewport(const LEMath::IntRect& rect)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_SETVIEWPORT(rect);
}

void DrawCommand::SetScissorRect(const LEMath::IntRect& rect)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_SETSCISSORRECT(rect);
}

void DrawCommand::BindVertexBuffer(VertexBufferGeneric *VertexBuffer)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDVERTEXBUFFER(VertexBuffer);
}

void DrawCommand::BindIndexBuffer(IndexBuffer *InIndexBuffer)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDINDEXBUFFER(InIndexBuffer);
}

void DrawCommand::Dispatch(int x, int y, int z)
{
	COMMANDBUFFER_NEW CommandBuffer::COMMAND_DISPATCH(x, y, z);
}

void DrawCommand::DrawPrimitive(RendererFlag::PrimitiveTypes type, uint32 offset, uint32 count)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_DRAWPRIMITIVE(type, offset, count);
}

void DrawCommand::DrawIndexedPrimitive(RendererFlag::PrimitiveTypes type, uint32 vtxcount, uint32 count)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_DRAWINDEXEDPRIMITIVE(type, vtxcount, count);
}

void DrawCommand::SetPipelineState(PipelineState *pso)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_SETPIPELINESTATE(pso);
}

void DrawCommand::UpdateConstantBuffer(ConstantBuffer* buffer, void* data, size_t size)
{
    CommandBuffer::COMMAND_UPDATECONSTANTBUFFER* currentCommand = COMMANDBUFFER_NEW CommandBuffer::COMMAND_UPDATECONSTANTBUFFER(buffer, LE_DrawManagerRendererAccessor.GetCommandBuffer()->duplicateBuffer(data, size), size);
    currentCommand->nextOffset = LE_DrawManagerRendererAccessor.GetCommandBuffer()->CalculateCommandOffset((CommandBuffer::COMMAND*)currentCommand);
}

void DrawCommand::SetConstantBuffer(uint32 idx, ConstantBuffer* buffer)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_SETCONSTANTBUFFER(idx, buffer);
}

void DrawCommand::SetRenderTarget(uint32 index, TextureInterface* color, TextureInterface* depth, uint32 surfaceIndex)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_SETRENDERTARGET(index, color, depth, surfaceIndex);
}

void DrawCommand::ResourceBarrier(TextureInterface *InTexture, const ResourceState &InResourceState)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_RESOURCEBARRIER(InTexture, InResourceState);
}

void DrawCommand::ResourceBarrier(IndexBuffer* InIndexBuffer, const ResourceState& InResourceState)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_RESOURCEBARRIER(InIndexBuffer, InResourceState);
}

void DrawCommand::ResourceBarrier(VertexBufferGeneric* InVertexBuffer, const ResourceState& InResourceState)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_RESOURCEBARRIER(InVertexBuffer, InResourceState);
}

void DrawCommand::BindTargetTexture(uint32 index, Texture *texture)
{
	COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDTARGETTEXTURE(index, texture);
}

void DrawCommand::BindSampler(uint32 index, SamplerState *sampler)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDSAMPLER(index, sampler);
}

void DrawCommand::BindTexture(uint32 index, Texture *texture)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDTEXTURE(index, texture);
}

void DrawCommand::BindTexture(uint32 index, const PooledRenderTarget &texture)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDPOOLEDRENDERTARGET(index, texture);
}

void DrawCommand::BindTexture(uint32 index, const PooledDepthStencil &texture)
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_BINDPOOLEDDEPTHSTENCIL(index, texture);
}

void DrawCommand::SetMarker(const char *InMarkerName)
{
    CommandBuffer::COMMAND_SETMARKER *currentCommand = COMMANDBUFFER_NEW CommandBuffer::COMMAND_SETMARKER(LE_DrawManagerRendererAccessor.GetCommandBuffer()->duplicateString(InMarkerName));
    currentCommand->nextOffset = LE_DrawManagerRendererAccessor.GetCommandBuffer()->CalculateCommandOffset((CommandBuffer::COMMAND*)currentCommand);
}

void DrawCommand::BeginEvent(const char *InEventName)
{
    CommandBuffer::COMMAND_BEGINEVENT *currentCommand = COMMANDBUFFER_NEW CommandBuffer::COMMAND_BEGINEVENT(LE_DrawManagerRendererAccessor.GetCommandBuffer()->duplicateString(InEventName));
    currentCommand->nextOffset = LE_DrawManagerRendererAccessor.GetCommandBuffer()->CalculateCommandOffset((CommandBuffer::COMMAND*)currentCommand);
}

void DrawCommand::EndEvent()
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_ENDEVENT();
}

void DrawCommand::Present()
{
    COMMANDBUFFER_NEW CommandBuffer::COMMAND_PRESENT();
}
}