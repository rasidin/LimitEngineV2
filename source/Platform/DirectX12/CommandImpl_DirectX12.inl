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
@author minseob (https://github.com/rasidin)
**********************************************************************/
#include <d3d12.h>

#include "PrivateDefinitions_DirectX12.h"

#include "Containers/VectorArray.h"

namespace LimitEngine {
    class CommandImpl_DirectX12 : public CommandImpl
    {
        class DescriptorHeapPool {
        public:
            enum class PoolType {
                SRVandUAVandCB = 0,
                Sampler = 1,
                Num
            };
            static constexpr uint32 PoolTypeCount = static_cast<uint32>(PoolType::Num);
        private:
            static constexpr uint32 DescriptorsCountPerHeap = 1024u;
            struct DescriptorHeapSet {
                uint64 Fence = 0u;
                ID3D12DescriptorHeap* Heap = nullptr;
            };
        public:
            ~DescriptorHeapPool() {
                for (uint32 ptidx = 0; ptidx < PoolTypeCount; ptidx++) {
                    for (auto *heappool : mUsedHeapPool[ptidx]) {
                        heappool->Release();
                    }
                    mUsedHeapPool[ptidx].Clear();
                    for (const DescriptorHeapSet& heappool : mFreeHeapPool[ptidx]) {
                        heappool.Heap->Release();
                    }
                    mFreeHeapPool[ptidx].Clear();
                }
            }
        public:
            ID3D12DescriptorHeap* GetNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) {
                uint32 ptidx = type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? static_cast<uint32>(PoolType::Sampler) : static_cast<uint32>(PoolType::SRVandUAVandCB);
                ID3D12DescriptorHeap* currentheap = nullptr;
                for (uint32 hpidx = 0; hpidx < mFreeHeapPool[ptidx].count(); hpidx++) {
                    if (mFreeHeapPool[ptidx][hpidx].Fence < mCurrentFenceIndex) {
                        currentheap = mFreeHeapPool[ptidx][hpidx].Heap;
                        mFreeHeapPool[ptidx].Delete(hpidx);
                        break;
                    }
                }
                if (!currentheap) {
                    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                    desc.Type = type;
                    desc.NumDescriptors = DescriptorsCountPerHeap;
                    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    desc.NodeMask = 1;
                    if (FAILED(((ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle())->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&currentheap)))) {
                        Debug::Error("[CommandImpl_Direct12] Failed to create descriptor heap");
                        return nullptr;
                    }
                }
                mUsedHeapPool[ptidx].Add(currentheap);
                return currentheap;
            }
            void Finalize(uint64 fence) {
                mCurrentFenceIndex = fence;
                for (uint32 ptidx = 0; ptidx < PoolTypeCount; ptidx++) {
                    for (ID3D12DescriptorHeap* heap : mUsedHeapPool[ptidx]) {
                        mFreeHeapPool[ptidx].Add({ fence, heap });
                    }
                    mUsedHeapPool[ptidx].Clear();
                }
            }
        private:
            uint64 mCurrentFenceIndex = 0u;
            VectorArray<ID3D12DescriptorHeap*> mUsedHeapPool[PoolTypeCount];
            VectorArray<DescriptorHeapSet> mFreeHeapPool[PoolTypeCount];
        } mDescriptorHeapPool;
        struct Cache
        {
            static constexpr uint32 CachedConstantBufferMaxNum = 16u;
            static constexpr uint32 CachedSamplerMaxNum = 16u;
            static constexpr uint32 CachedTextureMaxNum = 16u;

            Shader                      *CurrentShader;
            ConstantBuffer              *CurrentConstantBuffers[CachedConstantBufferMaxNum];
            D3D12_CPU_DESCRIPTOR_HANDLE  CurrentSamplers[CachedSamplerMaxNum];
            D3D12_CPU_DESCRIPTOR_HANDLE  CurrentSRVs[CachedTextureMaxNum];
            Cache() { Clear(); }
            void Clear()
            {
                CurrentShader = nullptr;
                ::memset(CurrentConstantBuffers, 0, sizeof(CurrentConstantBuffers));
                ::memset(CurrentSamplers, 0, sizeof(CurrentSamplers));
                ::memset(CurrentSRVs, 0, sizeof(CurrentSRVs));
            }
            bool IsValid() const { return true; }
            void Finalize(DescriptorHeapPool &HeapPool, ID3D12GraphicsCommandList *CommandList)
            {
                uint32 heapstobindnum = 0u;
                ID3D12DescriptorHeap* heapstobind[DescriptorHeapPool::PoolTypeCount] = { nullptr, nullptr };
                D3D12_CPU_DESCRIPTOR_HANDLE cpudescs[DescriptorHeapPool::PoolTypeCount] = { {nullptr}, {nullptr} };
                D3D12_GPU_DESCRIPTOR_HANDLE gpudescs[DescriptorHeapPool::PoolTypeCount] = { {nullptr}, {nullptr} };

                uint32 cbcount = 0u;
                uint32 cbrangesizes[CachedConstantBufferMaxNum] = {};
                for (uint32 cbidx = 0; cbidx < CachedConstantBufferMaxNum; cbidx++) {
                    if (CurrentConstantBuffers[cbidx]) {
                        cbrangesizes[cbidx] = 1;
                        cbcount++;
                    }
                }

                uint32 samplercount = 0u;
                uint32 samplerrangesizes[CachedSamplerMaxNum] = {};
                for (uint32 ssidx = 0; ssidx < CachedSamplerMaxNum; ssidx++) {
                    if (CurrentSamplers[ssidx].ptr) {
                        samplerrangesizes[ssidx] = 1;
                        samplercount++;
                    }
                }
                uint32 srvcount = 0u;
                uint32 srcrangesizes[CachedTextureMaxNum] = {};
                for (uint32 srvidx = 0; srvidx < CachedTextureMaxNum; srvidx++) {
                    if (CurrentSRVs[srvidx].ptr) {
                        srcrangesizes[srvidx] = 1;
                        srvcount++;
                    }
                }
                const bool bHasSamplers = samplercount > 0u;
                const bool bHasSRVs = srvcount > 0u;
                if (bHasSamplers) {
                    heapstobind[heapstobindnum] = HeapPool.GetNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                    cpudescs[heapstobindnum] = heapstobind[heapstobindnum]->GetCPUDescriptorHandleForHeapStart();
                    gpudescs[heapstobindnum] = heapstobind[heapstobindnum]->GetGPUDescriptorHandleForHeapStart();
                    ++heapstobindnum;
                }
                if (bHasSRVs) {
                    heapstobind[heapstobindnum] = HeapPool.GetNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    cpudescs[heapstobindnum] = heapstobind[heapstobindnum]->GetCPUDescriptorHandleForHeapStart();
                    gpudescs[heapstobindnum] = heapstobind[heapstobindnum]->GetGPUDescriptorHandleForHeapStart();
                    ++heapstobindnum;
                }
                CommandList->SetDescriptorHeaps(heapstobindnum, heapstobind);

                ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
                if (heapstobindnum > 0) {
                    UINT paramindex = cbcount;
                    UINT descindex = 0u;
                    if (bHasSamplers) {
                        CommandList->SetGraphicsRootDescriptorTable(paramindex++, gpudescs[descindex]);
                        device->CopyDescriptors(
                            1, &cpudescs[descindex], &samplercount,
                            samplercount, CurrentSamplers, samplerrangesizes, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
                        );
                        descindex++;
                    }
                    if (bHasSRVs) {
                        int srvidx = cbcount + samplercount;
                        CommandList->SetGraphicsRootDescriptorTable(paramindex++, gpudescs[descindex]);
                        device->CopyDescriptors(
                            1, &cpudescs[descindex], &srvcount,
                            srvcount, CurrentSRVs, srcrangesizes, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
                        );
                        descindex++;
                    }
                }
            }
        } mCache;
    public:
        CommandImpl_DirectX12()
            : CommandImpl()
        {}
        virtual ~CommandImpl_DirectX12() {
            for (uint32 cmdidx = 0; cmdidx < CommandListCount; cmdidx++) {
                if (mPooledD3DGraphicsCommandList[cmdidx])
                    mPooledD3DGraphicsCommandList[cmdidx]->Release();
                if (mPooledD3DCommandAllocator[cmdidx])
                    mPooledD3DCommandAllocator[cmdidx]->Release();
            }
        }

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
        void Finalize(uint64 CompletedFenceValue) override
        {
            mDescriptorHeapPool.Finalize(CompletedFenceValue);
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
        void SetViewport(const LEMath::IntRect& rect) override 
        {  
            D3D12_VIEWPORT viewport{(FLOAT)rect.X(), (FLOAT)rect.Y(), (FLOAT)rect.Width(), (FLOAT)rect.Height(), 0.0f, 1.0f};
            mD3DGraphicsCommandList->RSSetViewports(1, &viewport);
        }
        void SetScissorRect(const LEMath::IntRect& rect) override
        {
            D3D12_RECT screct{rect.X(), rect.Y(), rect.X() + rect.Width(), rect.Y() + rect.Height()};
            mD3DGraphicsCommandList->RSSetScissorRects(1, &screct);
        }
        bool PrepareForDrawing() override
        {
            if (!mCache.IsValid()) return false;

            mCache.Finalize(mDescriptorHeapPool, mD3DGraphicsCommandList);

            return true;
        }
        void ClearCaches() override
        {
            mCache.Clear();
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
        void BindVertexBuffer(VertexBufferGeneric *vb) override
        {
            if (!vb) return;

            if (mD3DGraphicsCommandList) {
                VertexBufferRendererAccessor vbra(vb);
                ID3D12Resource* VertexBufferHandle = (ID3D12Resource*)vbra.GetHandle();
                if (VertexBufferHandle == nullptr) return;
                //if (Buffer) {
                //    D3D12_RANGE WriteRange = {0, 0};
                //    void* pMappedVertexBuffer = nullptr;
                //    if (SUCCEEDED(VertexBufferHandle->Map(0, &WriteRange, &pMappedVertexBuffer))) {
                //        memcpy(pMappedVertexBuffer, Buffer, Size);
                //        VertexBufferHandle->Unmap(0, &WriteRange);
                //    }
                //}
                D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
                VertexBufferView.BufferLocation = VertexBufferHandle->GetGPUVirtualAddress();
                VertexBufferView.StrideInBytes = vbra.GetStride();
                VertexBufferView.SizeInBytes = vbra.GetBufferSize();
                mD3DGraphicsCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
            }
        }
        void BindIndexBuffer(IndexBuffer *ib) override
        {
            if (!ib) return;

            if (mD3DGraphicsCommandList) {
                IndexBufferRendererAccessor ibra(ib);
                mD3DGraphicsCommandList->IASetIndexBuffer((D3D12_INDEX_BUFFER_VIEW*)ibra.GetHandle());
            }
        }
        void SetConstantBuffer(uint32 Index, ConstantBuffer *InConstantBuffer) override
        {
            if (InConstantBuffer) {
                mCache.CurrentConstantBuffers[Index] = InConstantBuffer;
                if (ID3D12Resource* resource = static_cast<ID3D12Resource*>(ConstantBufferRendererAccessor(InConstantBuffer).GetResource())) {
                    mD3DGraphicsCommandList->SetGraphicsRootConstantBufferView(Index, resource->GetGPUVirtualAddress());
                }
            }
        }
        void SetPipelineState(PipelineState *pso) override
        {
            if (pso->IsValid()) {
                if (ID3D12RootSignature* handle = static_cast<ID3D12RootSignature*>(PipelineStateRendererAccessor(pso).GetRootSignatureHandle())) {
                    mD3DGraphicsCommandList->SetGraphicsRootSignature(handle);
                }
                if (ID3D12PipelineState* handle = static_cast<ID3D12PipelineState*>(PipelineStateRendererAccessor(pso).GetHandle())) {
                    mD3DGraphicsCommandList->SetPipelineState(handle);
                }
            }
        }
        void UpdateConstantBuffer(ConstantBuffer* cb, void* data, size_t size) override
        {
            if (cb) ConstantBufferRendererAccessor(cb).Update(data);
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
            mCache.CurrentSamplers[Index] = *((D3D12_CPU_DESCRIPTOR_HANDLE*)SamplerStateRendererAccessor(Sampler).GetHandle());
        }
        void BindTexture(uint32 Index, TextureInterface *InTexture) override
        {
            if (Index == 0xffffffff) return;

            mCache.CurrentSRVs[Index] = *(D3D12_CPU_DESCRIPTOR_HANDLE*)TextureRendererAccessor(InTexture).GetShaderResourceView();
        }
        void Dispatch(int X, int Y, int Z) override
        {
        }
        void DrawPrimitive(uint32 Primitive, uint32 Offset, uint32 Count) override
        {
            float blendfactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            mD3DGraphicsCommandList->OMSetBlendFactor(blendfactor);

            mD3DGraphicsCommandList->IASetPrimitiveTopology(PrimitiveTopologyTypeToD3DPrimitiveTopology[Primitive]);
            mD3DGraphicsCommandList->DrawInstanced(Count, 1, Offset, 0);
            ClearCaches();
        }
        void DrawIndexedPrimitive(uint32 Primitive, uint32 VertexCount, uint32 Count) override
        {
            float blendfactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            mD3DGraphicsCommandList->OMSetBlendFactor(blendfactor);

            mD3DGraphicsCommandList->IASetPrimitiveTopology(PrimitiveTopologyTypeToD3DPrimitiveTopology[Primitive]);
            mD3DGraphicsCommandList->DrawIndexedInstanced(Count, 1, 0, 0, 0);
            ClearCaches();
        }
        void SetRenderTarget(uint32 Index, const TextureRendererAccessor& Color, const TextureRendererAccessor& Depth, uint32 SurfaceIndex) override
        {
            LEASSERT(Color.IsValid());
            if (mD3DDevice) {
                mD3DGraphicsCommandList->OMSetRenderTargets(
                    1, 
                    (D3D12_CPU_DESCRIPTOR_HANDLE*)Color.GetRenderTargetView(),
                    false, 
                    (D3D12_CPU_DESCRIPTOR_HANDLE*)Depth.GetDepthStencilView());
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
                ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
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
            mD3DGraphicsCommandList->SetMarker(0, InMarkerName, static_cast<uint32>(strlen(InMarkerName) + 1));
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