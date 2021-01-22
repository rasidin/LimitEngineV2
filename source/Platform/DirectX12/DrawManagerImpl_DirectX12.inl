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
@file  DrawManagerImpl_DirectX12.inl
@brief Main File for Windows
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include <wrl.h>
#include <dxgi1_4.h>

#include <LERenderer>

#include "Core/Debug.h"
#include "PrivateDefinitions_DirectX12.h"

#include "Containers/VectorArray.h"
#include "Renderer/CommandBuffer.h"

namespace LimitEngine {
    class DescriptorAllocator
    {
    public:
        static constexpr uint32 DescriptorsPerHeap = 0xffu;

    public:
        DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE InType)
            : mType(InType)
        {}
        void Init(ID3D12Device *Device)
        {
            mD3DDevice = Device;

            GenerateNewDescriptorHeaps();
            mCurrentDescriptorHandle = mDescriptorHeaps[0]->GetCPUDescriptorHandleForHeapStart();
            mDescriptorHandleSize = Device->GetDescriptorHandleIncrementSize(mType);
        }
        void Term()
        {
            for (ID3D12DescriptorHeap* heap : mDescriptorHeaps) {
                if (heap)
                    heap->Release();
            }
            mDescriptorHeaps.Clear();
        }
        D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32 Count = 1)
        {
            if (Count > mRemainHandleCount) {
                GenerateNewDescriptorHeaps();
                mCurrentDescriptorHandle = mDescriptorHeaps.Last()->GetCPUDescriptorHandleForHeapStart();
            }
            D3D12_CPU_DESCRIPTOR_HANDLE Output = mCurrentDescriptorHandle;
            mCurrentDescriptorHandle.ptr += Count * mDescriptorHandleSize;
            mRemainHandleCount -= Count;

            return Output;
        }

    private:
        void GenerateNewDescriptorHeaps()
        {
            D3D12_DESCRIPTOR_HEAP_DESC Desc;
            Desc.Type = mType;
            Desc.NumDescriptors = DescriptorsPerHeap;
            Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            Desc.NodeMask = 1;

            ID3D12DescriptorHeap* NewHeap = nullptr;
            if (FAILED(mD3DDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&NewHeap)))) {
                Debug::Error("Failed to create heap for descriptors");
                return;
            }
            mDescriptorHeaps.Add(NewHeap);

            mRemainHandleCount = DescriptorsPerHeap;
        }

    private:
        ID3D12Device                        *mD3DDevice;
        D3D12_DESCRIPTOR_HEAP_TYPE           mType;
        uint32                               mDescriptorHandleSize;
        int32                                mRemainHandleCount;
        VectorArray<ID3D12DescriptorHeap*>   mDescriptorHeaps;
        D3D12_CPU_DESCRIPTOR_HANDLE          mCurrentDescriptorHandle;

    };
    class DrawManagerImpl_DirectX12 : public DrawManagerImpl
    {
        enum {
            SWAP_CHAIN_BUFFER_COUNT = 2,
        };
        static constexpr uint32 DescriptorsPerHeap = 0xffu;
    public:
        DrawManagerImpl_DirectX12()
        {
            for (uint64 queueIndex = 0; queueIndex < CommandQueueTypeNum; queueIndex++) {
                mCommandQueue[queueIndex] = nullptr;
                mCommandQueueFence[queueIndex] = nullptr;
                mCommandQueueFenceValue[queueIndex] = 1;
            }
            for (uint32 swapChainIndex = 0; swapChainIndex < SWAP_CHAIN_BUFFER_COUNT; swapChainIndex++) {
                mDisplayBuffers[swapChainIndex] = nullptr;
            }
        }
        virtual ~DrawManagerImpl_DirectX12()
        {}

        void* MakeInitParameter() override 
        { 
            CommandInit_Parameter *OutParameter = new CommandInit_Parameter();

            OutParameter->mD3DDevice = mD3DDevice;
            for (uint32 cqType = 0, cqCount = CommandQueueTypeNum; cqType < cqCount; cqType++) {
                OutParameter->mCommandQueue[cqType]      = mCommandQueue[cqType];
                OutParameter->mCommandQueueFence[cqType] = mCommandQueueFence[cqType];
            }

            return (void*)OutParameter;
        }

        void ReadyToRender() override {}

        virtual LEMath::IntSize GetScreenSize() override
        {
            return mDisplayBufferSize;
        }

        void PreRenderFinished() override {}

        void Init(WINDOW_HANDLE handle, const InitializeOptions& Options) override
        {
#if _DEBUG
            Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
            if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
                Debug::Error("Failed to create debug interface");
            }
            else {
                debugInterface->EnableDebugLayer();
                ID3D12Debug1 *debugcontroller1 = nullptr;
                debugInterface->QueryInterface(IID_PPV_ARGS(&debugcontroller1));
                debugcontroller1->SetEnableGPUBasedValidation(true);
            }
#endif
            Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
            if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)))) {
                Debug::Error("Failed to create factory");
                return;
            }

            Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
            if (FAILED(dxgiFactory->EnumAdapters1(0, &adapter))) {
                Debug::Error("Failed to enum adapters");
                return;
            }

            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Create device
            if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3DDevice)))) {
                Debug::Error("Failed to create device");
                return;
            }

            // Create command buffer
            for (uint32 cqType = 0, cqCount = static_cast<uint32>(CommandQueueType::Num); cqType < cqCount; cqType++) {
                D3D12_COMMAND_QUEUE_DESC queueDesc = {};
                queueDesc.Type = CommandQueueTypeToD3D12[cqType];
                queueDesc.NodeMask = 1;
                mD3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue[cqType]));
                mCommandQueue[cqType]->SetName(CommandQueueTypeName[cqType]);
                mD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mCommandQueueFence[cqType]));
                mCommandQueueFence[cqType]->SetName(CommandQueueTypeFenceName[cqType]);
                mCommandQueueFence[cqType]->Signal(cqType);
            }

            for (uint32 HeapArrayIndex = 0; HeapArrayIndex < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; HeapArrayIndex++) {
                mDescriptorAllocator[HeapArrayIndex].Init(mD3DDevice);
            }

            // Create swap chain
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width = Options.Resolution.Width();
            swapChainDesc.Height = Options.Resolution.Height();
            swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
            if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
                mCommandQueue[static_cast<uint32>(CommandQueueType::Graphics)], 
                handle,
                &swapChainDesc,
                nullptr,
                nullptr,
                &mDXGISwapChain
                ))) {
                Debug::Error("Failed to create swap chain");
                return;
            }
            IDXGISwapChain2* swapchain2 = nullptr;
            if (SUCCEEDED(mDXGISwapChain->QueryInterface(&swapchain2))) {
                mFrameLatencyWaitableObject = swapchain2->GetFrameLatencyWaitableObject();
            }

            mDisplayBufferSize = { Options.Resolution.Width(), Options.Resolution.Height() };
            mDisplayBufferFormat = RendererFlag::BufferFormat::R8G8B8A8_UNorm;
            for (uint32 BufIndex = 0; BufIndex < SWAP_CHAIN_BUFFER_COUNT; BufIndex++) {
                if (FAILED(mDXGISwapChain->GetBuffer(BufIndex, IID_PPV_ARGS(&mDisplayBuffers[BufIndex])))) {
                    Debug::Error("Failed to create swap chain buffer");
                    return;
                }
                mDisplayRenderTargetViews[BufIndex] = mDescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].Allocate();
                mD3DDevice->CreateRenderTargetView(mDisplayBuffers[BufIndex], nullptr, mDisplayRenderTargetViews[BufIndex]);
                mDisplayBufferResourceStates[BufIndex] = ResourceState::Present;
            }
        }

        void ResizeScreen(const LEMath::IntSize& size) override { /* Unimplemented */ }

        void ProcessBeforeFlushingCommands() override
        {
            if (mFrameLatencyWaitableObject) {
                WaitForSingleObjectEx(mFrameLatencyWaitableObject, 1000, true);
            }
        }

        void Finish(CommandBuffer* commandBuffer) override
        {
            ID3D12GraphicsCommandList* graphicsCommandList = (ID3D12GraphicsCommandList*)commandBuffer->GetCommandListHandle();
            graphicsCommandList->Close();

            // Wait last fence
            if (mCommandQueueFenceValue[(uint32)CommandQueueType::Graphics] > 1) {
                uint64 lastFenceValue = mCommandQueueFence[(uint32)CommandQueueType::Graphics]->GetCompletedValue();
                while (mCommandQueueFenceValue[(uint32)CommandQueueType::Graphics] - 1 > lastFenceValue) {
                    lastFenceValue = mCommandQueueFence[(uint32)CommandQueueType::Graphics]->GetCompletedValue();
                }
            }

            commandBuffer->ReadyToExecute();

            ID3D12CommandList* commandList = (ID3D12CommandList*)graphicsCommandList;
            mCommandQueue[(uint32)CommandQueueType::Graphics]->ExecuteCommandLists(1, &commandList);
            mCommandQueue[(uint32)CommandQueueType::Graphics]->Signal(mCommandQueueFence[(uint32)CommandQueueType::Graphics], mCommandQueueFenceValue[(uint32)CommandQueueType::Graphics]);

            mCommandQueueFenceValue[(uint32)CommandQueueType::Graphics]++;
        }

        void Present()
        {
            mDXGISwapChain->Present(0, 0);
            mCurrentFrameBufferIndex = (mCurrentFrameBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
        }

        void Term() override
        {
            for (uint32 BufIndex = 0; BufIndex < SWAP_CHAIN_BUFFER_COUNT; BufIndex++) {
                if (mDisplayBuffers[BufIndex] != nullptr) {
                    mDisplayBuffers[BufIndex]->Release();
                    mDisplayBuffers[BufIndex] = nullptr;
                }
            }
            for (uint32 HeapArrayIndex = 0; HeapArrayIndex < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; HeapArrayIndex++) {
                mDescriptorAllocator[HeapArrayIndex].Term();
            }

#if _DEBUG
            ID3D12DebugDevice* debugInterface;
            if (SUCCEEDED(mD3DDevice->QueryInterface(&debugInterface))) {
                debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
                debugInterface->Release();
            }
#endif
            mD3DDevice->Release();
        }

        uint32 GetCurrentFrameBufferIndex() const { return mCurrentFrameBufferIndex; }
        const RendererFlag::BufferFormat& GetCurrentFrameBufferFormat() const override { return mDisplayBufferFormat; }
        const ResourceState GetCurrentFrameBufferResourceState() const override { return mDisplayBufferResourceStates[mCurrentFrameBufferIndex]; }
        void SetCurrentFrameBufferResourceState(ResourceState state) { mDisplayBufferResourceStates[mCurrentFrameBufferIndex] = state; }
        void* GetCurrentFrameBuffer() const override { return mDisplayBuffers[mCurrentFrameBufferIndex]; }
        void* GetCurrentFrameBufferView() const override { return (void*)&mDisplayRenderTargetViews[mCurrentFrameBufferIndex]; }
        void* GetDeviceHandle() const override { return mD3DDevice; }
        void* GetDeviceContext() const override { return nullptr; }

    private:
        uint32                               mCurrentFrameBufferIndex = 0u;

        ID3D12Device                        *mD3DDevice = nullptr;
        ID3D12CommandQueue                  *mCommandQueue[CommandQueueTypeNum];
        ID3D12Fence                         *mCommandQueueFence[CommandQueueTypeNum];
        uint64                               mCommandQueueFenceValue[CommandQueueTypeNum];
        IDXGISwapChain1                     *mDXGISwapChain = nullptr;
        HANDLE                               mFrameLatencyWaitableObject = nullptr;
        LEMath::IntSize                      mDisplayBufferSize;
        RendererFlag::BufferFormat           mDisplayBufferFormat = RendererFlag::BufferFormat::Unknown;
        ID3D12Resource                      *mDisplayBuffers[SWAP_CHAIN_BUFFER_COUNT];
        D3D12_CPU_DESCRIPTOR_HANDLE          mDisplayRenderTargetViews[SWAP_CHAIN_BUFFER_COUNT];
        ResourceState                        mDisplayBufferResourceStates[SWAP_CHAIN_BUFFER_COUNT];

        DescriptorAllocator                  mDescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV
        };
    };
}
