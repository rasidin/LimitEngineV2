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
@file  IndexBufferImpl_DirectX12.inl
@brief IndexBuffer for rendering
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#pragma once
#include <d3d12.h>

#include "Core/Debug.h"

namespace LimitEngine {
class IndexBufferImpl_DirectX12 : public IndexBufferImpl
{
public:
    IndexBufferImpl_DirectX12(IndexBuffer *InOwner)
        : IndexBufferImpl(InOwner)
        , mIndexBuffer(nullptr)
    {}
    virtual ~IndexBufferImpl_DirectX12()
    {}

    virtual void Create(size_t Count, void* Buffer) override
    {
        size_t BufferSize = Count * sizeof(uint32);

        ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
        LEASSERT(device != nullptr);

        D3D12_HEAP_PROPERTIES HeapPropertiesDefault;
        HeapPropertiesDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
        HeapPropertiesDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        HeapPropertiesDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        HeapPropertiesDefault.CreationNodeMask = 1;
        HeapPropertiesDefault.VisibleNodeMask = 1;
        D3D12_RESOURCE_DESC ResourceDescDefault = {
            D3D12_RESOURCE_DIMENSION_BUFFER,
            0,
            BufferSize,
            1, 1, 1,
            DXGI_FORMAT_UNKNOWN,
            1, 0,
            D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            D3D12_RESOURCE_FLAG_NONE };
        if (FAILED(device->CreateCommittedResource(
            &HeapPropertiesDefault,
            D3D12_HEAP_FLAG_NONE,
            &ResourceDescDefault,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&mIndexBuffer)))) {
            Debug::Error("[IndexBuffer]Failed to create index buffer");
            return;
        }

        IndexBufferRendererAccessor(mOwner).SetResourceState(ResourceState::GenericRead);

        ID3D12Resource* indexBufferUpdate;
        D3D12_HEAP_PROPERTIES HeapPropertiesUpload;
        HeapPropertiesDefault.Type = D3D12_HEAP_TYPE_UPLOAD;
        HeapPropertiesDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        HeapPropertiesDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        HeapPropertiesDefault.CreationNodeMask = 1;
        HeapPropertiesDefault.VisibleNodeMask = 1;
        if (FAILED(device->CreateCommittedResource(
            &HeapPropertiesUpload,
            D3D12_HEAP_FLAG_NONE,
            &ResourceDescDefault,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&indexBufferUpdate)))) {
            mIndexBuffer->Release();
            mIndexBuffer = nullptr;
            Debug::Error("[IndexBuffer]Failed to create index buffer (for uploading)");
            return;
        }

        //DrawCommand::CopyBuffer((void*)mIndexBuffer, 0, (void*)indexBufferUpdate, 0, BufferSize);
        //DrawCommand::ResourceBarrier(mOwner, ResourceState::VertexAndConstantBuffer | ResourceState::IndexBuffer);
        
        mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
        mIndexBufferView.SizeInBytes = BufferSize;
        mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    }

    virtual void Dispose() override
    {
        if (mIndexBuffer)
            mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }

    virtual void* GetHandle() override
    {
        return &mIndexBufferView;
    }

    virtual void* GetResource() const override
    {
        return (void*)mIndexBuffer;
    }

    virtual void* GetBuffer() override
    {
        return mIndexBuffer;
    }
private:
    D3D12_INDEX_BUFFER_VIEW      mIndexBufferView;
    ID3D12Resource              *mIndexBuffer;
};
}