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
@file  LE_ConstantBufferImpl_DirectX12.inl
@brief ConstantBuffer Implement (DX12)
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include <d3d12.h>

#include "Managers/DrawManager.h"

namespace LimitEngine {
class ConstantBufferImpl_DirectX12 : public ConstantBufferImpl
{
public:
    ConstantBufferImpl_DirectX12() {}
    virtual ~ConstantBufferImpl_DirectX12() {
        if (mResource) {
            mResource->Release();
            mResource = nullptr;
        }
    }

    void Create(size_t size, void *initdata) override
    {
        mResource = static_cast<ID3D12Resource*>(LE_DrawManagerRendererAccessor.AllocateGPUBuffer(size));
        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

        if (initdata) {
            D3D12_RANGE range = { 0, 0 };
            void* mappeddata = nullptr;
            if (SUCCEEDED(mResource->Map(0, &range, &mappeddata))) {
                ::memcpy(mappeddata, initdata, size);
                mResource->Unmap(0, nullptr);
            }
        }
/*      // This code is
        D3D12_RESOURCE_DESC desc = {};
        desc.Alignment = 0;
        desc.DepthOrArraySize = 1;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Height = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Width = size;

        D3D12_DEAP_PROPERTIES heapprops;
        heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapprops.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapprops.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        ID3D12Device* device = static_cast<ID3D12Device*>(LE_DrawManagerRendererAccessor.GetDeviceHandle());
        if (FAILED(device->CreateCommittedResource(
            &heapprops, 
            D3D12_HEAP_FLAG_NONE, 
            &desc, 
            initdata ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_COMMON,
            nullptr, 
            IID_PPV_ARGS(&mResource)))) {
            Debug::Error("Failed to crate constant buffer");
            return;
        }

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvdesc = {};
        cbvdesc.BufferLocation = mGPUVirtualAddress;
        cbvdesc.SizeInBytes = size;
        mConstantBufferView = LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        device->CreateConstantBufferView(&cbvdesc, mConstantBufferView);

        if (initdata) {
            ID3D12GraphicsCommandList* cmdlist = static_cast<ID3D12GraphicsCommandList*>(LE_DrawManagerRendererAccessor.GetImmediateCommandList());
            ID3D12Resource* resource = static_cast<ID3D12Resource*>(LE_DrawManagerRendererAccessor.AllocateGPUBuffer(initDataSize));

            D3D12_RANGE range = { 0, 0 };
            void* mappeddata = nullptr;
            if (SUCCEEDED(resource->Map(0, &range, &mappeddata))) {
                ::memcpy(mappeddata, initdata, size);
                resource->Unmap(0, nullptr);
            }

            cmdlist->CopyBufferRegion(mResource, 0, resource, 0, size);

            D3D12_RESOURCE_BARRIER barrier;
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = mResource;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            cmdlist->ResourceBarrier(1, &barrier);

            LE_DrawManagerRendererAccessor.ExecuteImmediateCommandList(cmdlist, true);

            resource->Release();
        }
*/
    }

    void* GetResource() const override { return static_cast<void*>(mResource); }
    void* GetConstantBufferView() const override { return (void*)&mConstantBufferView; }
private:
    ID3D12Resource* mResource = nullptr;
    uint64 mGPUVirtualAddress = 0u;
    D3D12_CPU_DESCRIPTOR_HANDLE mConstantBufferView;
};
} // namespace LimitEngine