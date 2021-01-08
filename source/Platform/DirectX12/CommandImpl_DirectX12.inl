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
@file  CommandImpl_DirectX12.inl
@brief CommandBuffer for rendering
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include <d3d12.h>

#include "PrivateDefinitions_DirectX12.h"

namespace LimitEngine {
    class CommandImpl_DirectX12 : public CommandImpl
    {
        struct Cache
        {
            static constexpr uint32 CachedSamplerMaxNum = 16;

            Shader          *CurrentShader;
            ConstantBuffer  *CurrentConstantBuffer;
            SamplerState    *CurrentSamplers[CachedSamplerMaxNum];
            Cache()
                : CurrentShader(nullptr)
                , CurrentConstantBuffer(nullptr)
            {
                ::memset(CurrentSamplers, 0, sizeof(CurrentSamplers));
            }
        } mCache;
    public:
        CommandImpl_DirectX12()
            : CommandImpl()
        {}
        virtual ~CommandImpl_DirectX12() {}

        void Init(void* Parameter) override
        {
            if (Parameter == nullptr) return;

            ID3D12CommandQueue *GraphicsCommandQueue = nullptr;
            ID3D12Fence *GraphicsCommandQueueFence = nullptr;
            if (CommandInit_Parameter* InitParameter = (CommandInit_Parameter*)Parameter) {
                mD3DDevice = InitParameter->mD3DDevice;
                GraphicsCommandQueue = InitParameter->mCommandQueue[static_cast<int32>(CommandQueueType::Graphics)];
                GraphicsCommandQueueFence = InitParameter->mCommandQueueFence[static_cast<int32>(CommandQueueType::Graphics)];

                delete InitParameter;
            }

            ::memset(&mCurrentRenderTargetView, 0, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE));
            ::memset(&mCurrentDepthStencilView, 0, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE));

            uint64 CompletedFence = GraphicsCommandQueueFence->GetCompletedValue();
            for (uint32 CommandListIndex = 0; CommandListIndex < CommandListCount; CommandListIndex++) {
                if (FAILED(mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mPooledD3DCommandAllocator[CommandListIndex])))) {
                    Debug::Error("Failed to create command allocator.");
                    return;
                }
                mPooledD3DCommandAllocator[CommandListIndex]->SetName(L"GraphicsCommandAllocator");
                HRESULT hr = mD3DDevice->CreateCommandList(1, D3D12_COMMAND_LIST_TYPE_DIRECT, mPooledD3DCommandAllocator[CommandListIndex], nullptr, IID_PPV_ARGS(&mPooledD3DGraphicsCommandList[CommandListIndex]));
                if (FAILED(hr)) {
                    LEASSERT(0);
                }
            }
            mD3DCommandAllocator = mPooledD3DCommandAllocator[mCurrentCommandListIndex];
            mD3DGraphicsCommandList = mPooledD3DGraphicsCommandList[mCurrentCommandListIndex];

            mFirstFinish = true;
        }
        void Term() override
        {
            for (uint32 CommandListIndex = 0; CommandListIndex < CommandListCount; CommandListIndex++) {
                mPooledD3DCommandAllocator[CommandListIndex]->Release();
                mPooledD3DGraphicsCommandList[CommandListIndex]->Release();
                mPooledD3DCommandAllocator[CommandListIndex] = nullptr;
                mPooledD3DGraphicsCommandList[CommandListIndex] = nullptr;
            }
            mD3DGraphicsCommandList = nullptr;
            mD3DCommandAllocator = nullptr;

            for (ID3D12Resource* resource : mPendingReleaseResources) {
                resource->Release();
            }
            mPendingReleaseResources.Clear();
        }

        void* GetCommandListHandle() override
        {
            return mD3DGraphicsCommandList;
        }
        void ReadyToExecute() override
        {
        }
        void Finish() override
        {
            if (mCurrentFrameBufferResourceState != ResourceState::Present) {
                ID3D12Resource* colorFrameBuffer = (ID3D12Resource*)LE_DrawManager.GetCurrentFrameBuffer();
                ResourceBarrier(colorFrameBuffer, mCurrentFrameBufferResourceState, ResourceState::Present);

                mCurrentFrameBufferResourceState = ResourceState::Present;
            }
        }
        void ProcessAfterPresent() override
        {
            mCurrentCommandListIndex = (mCurrentCommandListIndex + 1) % CommandListCount;

            mD3DCommandAllocator = mPooledD3DCommandAllocator[mCurrentCommandListIndex];
            mD3DGraphicsCommandList = mPooledD3DGraphicsCommandList[mCurrentCommandListIndex];

            if (!mFirstFinish) {
                mD3DCommandAllocator->Reset();
                mD3DGraphicsCommandList->Reset(mD3DCommandAllocator, nullptr);
            }
            mFirstFinish = false;

            for (ID3D12Resource* resource : mPendingReleaseResources) {
                resource->Release();
            }
            mPendingReleaseResources.Clear();
        }

        void BeginScene() override {}
        void EndScene() override {}

        bool PrepareForDrawing() override
        {
            if (!mCache.CurrentShader) return false;

            ShaderImpl *pShaderImpl = mCache.CurrentShader->GetImplementation();
            
            D3D12_ROOT_SIGNATURE_DESC RootSigDesc;


            return true;
        }
        bool PrepareForDrawingModel() override { return true; }
        void ClearScreen(const LEMath::FloatColorRGBA& Color) override
        {
            if (mCurrentRenderTargetView.ptr) {
                mD3DGraphicsCommandList->ClearRenderTargetView(mCurrentRenderTargetView, (float*)&Color, 0, nullptr);
            }
            if (mCurrentDepthStencilView.ptr) {
                mD3DGraphicsCommandList->ClearDepthStencilView(mCurrentDepthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            }
        }
        void BindVertexBuffer(void *Handle, void *Buffer, uint32 Offset, uint32 Size, uint32 Stride) override
        {
            if (!Handle) return;

            if (mD3DGraphicsCommandList) {
                ID3D12Resource* VertexBufferHandle = (ID3D12Resource*)Handle;
                if (Buffer) {
                    D3D12_RANGE WriteRange = {0, 0};
                    void* pMappedVertexBuffer = nullptr;
                    if (SUCCEEDED(VertexBufferHandle->Map(0, &WriteRange, &pMappedVertexBuffer))) {
                        memcpy(pMappedVertexBuffer, Buffer, Size);
                        VertexBufferHandle->Unmap(0, &WriteRange);
                    }
                }
                D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
                VertexBufferView.BufferLocation = VertexBufferHandle->GetGPUVirtualAddress();
                VertexBufferView.StrideInBytes = Stride;
                VertexBufferView.SizeInBytes = Size;
                mD3DGraphicsCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
            }
        }
        void BindIndexBuffer(void* Handle, uint32 Size) override
        {
            if (!Handle) return;

            if (mD3DGraphicsCommandList) {
                ID3D12Resource* IndexResource = (ID3D12Resource*)Handle;
                D3D12_INDEX_BUFFER_VIEW IndexBufferView;
                IndexBufferView.BufferLocation = IndexResource->GetGPUVirtualAddress();
                IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
                IndexBufferView.SizeInBytes = Size;
            }
        }
        void BindConstantBuffer(ConstantBuffer *InConstantBuffer) override
        {
            mCache.CurrentConstantBuffer = InConstantBuffer;
        }
        void BindTargetTexture(uint32 Index, Texture* InTexture) override
        {
            if (Index == 0xffffffff) return;

            //if (mCache.CurrentShader) {
            //    if (mCache.CurrentShader->HasShader(Shader::Type::Compute)) {
            //        if (InTexture == nullptr) {
            //            mD3DGraphicsCommandList->SetComputeRootUnorderedAccessView(Index, nullptr);
            //        }
            //        else {
            //            ID3D12Resource* UAV = (ID3D12Resource*)InTexture->GetUnorderedAccessView();
            //            mD3DGraphicsCommandList->SetComputeRootUnorderedAccessView(Index, UAV->GetGPUVirtualAddress());
            //        }
            //    }
            //}
        }
        void BindSampler(uint32 Index, SamplerState *Sampler) override
        {
            mCache.CurrentSamplers[Index] = Sampler;
        }
        void BindTexture(uint32 Index, Texture *InTexture) override
        {
            if (Index == 0xffffffff) return;
        }
        void Dispatch(int X, int Y, int Z) override
        {
        }
        void DrawPrimitive(uint32 Primitive, uint32 Offset, uint32 Count) override
        {
        }
        void DrawIndexedPrimitive(uint32 Primitive, uint32 VertexCount, uint32 Count) override
        {}
        void SetFVF(uint32 FVF) override
        {}
        void SetCulling(uint32 Culling) override
        {}
        void SetBlendFunc(uint32 rt, RendererFlag::BlendFlags Func) override
        {}
        void SetDepthFunc(uint32 Func) override
        {}
        void SetEnabled(uint32 Flag) override
        {}
        void SetDisable(uint32 Flag) override
        {}
        void SetRenderTarget(uint32 Index, Texture *Color, Texture *Depth, uint32 SurfaceIndex) override
        {
            if (mD3DDevice) {
                if (!Color) {
                    if (mCurrentFrameBufferResourceState != ResourceState::RenderTarget) {
                        ID3D12Resource* colorFrameBuffer = (ID3D12Resource*)LE_DrawManager.GetCurrentFrameBuffer();
                        ResourceBarrier(colorFrameBuffer, mCurrentFrameBufferResourceState, ResourceState::RenderTarget);

                        mCurrentFrameBufferResourceState = ResourceState::RenderTarget;
                    }
                }

                mD3DGraphicsCommandList->OMSetRenderTargets(
                    1, 
                    Color?(D3D12_CPU_DESCRIPTOR_HANDLE*)Color->GetRenderTargetView():(D3D12_CPU_DESCRIPTOR_HANDLE*)LE_DrawManager.GetCurrentFrameBufferView(), 
                    false, 
                    Depth?(D3D12_CPU_DESCRIPTOR_HANDLE*)Depth->GetDepthStencilView():nullptr);
            }
        }
        void CopyResource(void* Dst, uint32 DstOffset, void* Org, uint32 OrgOffset, uint32 Size) override
        {
            if (!mD3DGraphicsCommandList) return;
            return;

            ID3D12Resource* DstResource = (ID3D12Resource*)Dst;
            ID3D12Resource* SrcResource = (ID3D12Resource*)Org;
            D3D12_RESOURCE_DESC DstDesc = DstResource->GetDesc();
            if (DstDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {
                mD3DGraphicsCommandList->CopyBufferRegion(DstResource, static_cast<uint64>(DstOffset), (ID3D12Resource*)Org, static_cast<uint64>(OrgOffset), Size);
            }
            else {
                D3D12_TEXTURE_COPY_LOCATION DstLocation = {
                    DstResource,
                    D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
                    0
                };
                ID3D12Device* device = (ID3D12Device*)LE_DrawManager.GetDeviceHandle();
                D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
                device->GetCopyableFootprints(&DstResource->GetDesc(), 0, 1, 0, &Footprint, nullptr, nullptr, nullptr);
                D3D12_TEXTURE_COPY_LOCATION SrcLocation = {
                    SrcResource,
                    D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
                    Footprint
                };
                mD3DGraphicsCommandList->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrcLocation, nullptr);
            }
            mPendingReleaseResources.Add((ID3D12Resource*)Org);
        }
        void ResourceBarrier(void* Resource, const ResourceState& Before, const ResourceState& After)
        {
            if (!mD3DGraphicsCommandList) return;

            D3D12_RESOURCE_BARRIER barrier;
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = (ID3D12Resource*)Resource;
            barrier.Transition.StateBefore = GetResourceStateToD3D12(Before);
            barrier.Transition.StateAfter = GetResourceStateToD3D12(After);
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            mD3DGraphicsCommandList->ResourceBarrier(1, &barrier);
        }
        void SetMarker(const char *InMarkerName) override
        {
            if (!mD3DGraphicsCommandList) return;
            mD3DGraphicsCommandList->SetMarker(0, InMarkerName, strlen(InMarkerName) + 1);
        }
        void BeginEvent(const char *InEventName) override
        {
            if (!mD3DGraphicsCommandList) return;
            //mD3DGraphicsCommandList->BeginEvent(0, InEventName, strlen(InEventName) + 1);
        }
        void EndEvent() override
        {
            if (!mD3DGraphicsCommandList) return;
            //mD3DGraphicsCommandList->EndEvent();
        }
        void* AllocateGPUBuffer(size_t size) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManager.GetDeviceHandle();
            LEASSERT(device);

            D3D12_RESOURCE_DESC desc = {};
            desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            desc.Width = size;
            desc.Height = 1;
            desc.DepthOrArraySize = 1;
            desc.MipLevels = 1;
            desc.SampleDesc.Count = 1;
            desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

            D3D12_HEAP_PROPERTIES heap = {};
            heap.Type = D3D12_HEAP_TYPE_UPLOAD;

            ID3D12Resource* resource = nullptr;
            if (SUCCEEDED(device->CreateCommittedResource(
                &heap,
                D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&resource)))) {
                return resource;
            }
            return nullptr;
        }
        void UploadToGPUBuffer(void* gpubuffer, void* data, size_t size) override
        {
            ID3D12Resource* resource = (ID3D12Resource*)gpubuffer;
            D3D12_RANGE range = { 0, 0 };
            void* mappedData = nullptr;
            if (SUCCEEDED(resource->Map(0, &range, &mappedData))) {
                ::memcpy(mappedData, data, size);
                resource->Unmap(0, nullptr);
            }
        }
    private:
        D3D12_RESOURCE_STATES GetResourceStateToD3D12(const ResourceState& State)
        {
            D3D12_RESOURCE_STATES Output = D3D12_RESOURCE_STATE_COMMON;
            if ((State & ResourceState::Common) == ResourceState::Common)                                   Output |= D3D12_RESOURCE_STATE_COMMON;
            if ((State & ResourceState::VertexAndConstantBuffer) == ResourceState::VertexAndConstantBuffer) Output |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            if ((State & ResourceState::IndexBuffer) == ResourceState::IndexBuffer)                         Output |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
            if ((State & ResourceState::RenderTarget) == ResourceState::RenderTarget)                       Output |= D3D12_RESOURCE_STATE_RENDER_TARGET;
            if ((State & ResourceState::UnorderedAccess) == ResourceState::UnorderedAccess)                 Output |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            if ((State & ResourceState::DepthWrite) == ResourceState::DepthWrite)                           Output |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
            if ((State & ResourceState::DepthRead) == ResourceState::DepthRead)                             Output |= D3D12_RESOURCE_STATE_DEPTH_READ;
            if ((State & ResourceState::NonPixelShaderResource) == ResourceState::NonPixelShaderResource)   Output |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            if ((State & ResourceState::PixelShaderResource) == ResourceState::PixelShaderResource)         Output |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            if ((State & ResourceState::StreamOut) == ResourceState::StreamOut)                             Output |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
            if ((State & ResourceState::CopyDest) == ResourceState::CopyDest)                               Output |= D3D12_RESOURCE_STATE_COPY_DEST;
            if ((State & ResourceState::CopySource) == ResourceState::CopySource)                           Output |= D3D12_RESOURCE_STATE_COPY_SOURCE;
            if ((State & ResourceState::ResolveDest) == ResourceState::ResolveDest)                         Output |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
            if ((State & ResourceState::ResolveSource) == ResourceState::ResolveSource)                     Output |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
            if ((State & ResourceState::GenericRead) == ResourceState::GenericRead)                         Output |= D3D12_RESOURCE_STATE_GENERIC_READ;
            if ((State & ResourceState::Present) == ResourceState::Present)                                 Output |= D3D12_RESOURCE_STATE_PRESENT;

            return Output;
        }
    private:
        static const uint32 CommandListCount = 2u;

        ID3D12Device                    *mD3DDevice;

        bool                             mFirstFinish = true;

        ResourceState                    mCurrentFrameBufferResourceState = ResourceState::Present;

        uint32                           mCurrentCommandListIndex = 0u;
        ID3D12GraphicsCommandList       *mPooledD3DGraphicsCommandList[CommandListCount];
        ID3D12CommandAllocator          *mPooledD3DCommandAllocator[CommandListCount];

        ID3D12GraphicsCommandList       *mD3DGraphicsCommandList;
        ID3D12CommandAllocator          *mD3DCommandAllocator;

        VectorArray<ID3D12Resource*>     mPendingReleaseResources;

        D3D12_CPU_DESCRIPTOR_HANDLE      mCurrentRenderTargetView;
        D3D12_CPU_DESCRIPTOR_HANDLE      mCurrentDepthStencilView;
    };
}