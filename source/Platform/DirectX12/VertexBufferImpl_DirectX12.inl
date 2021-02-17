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
---------------------------------------------------------------------
@file  VertexBufferImpl_DirectX12.inl
@brief VertexBuffer (DirectX12)
@author minseob (https://github.com/rasidin)
*********************************************************************/
#include <d3d12.h>
#include "Renderer/DrawCommand.h"

namespace LimitEngine {
    class VertexBufferImpl_DirectX12 : public VertexBufferImpl
    {
    public:
        VertexBufferImpl_DirectX12() {}
        virtual ~VertexBufferImpl_DirectX12() {}

        void Create(uint32 fvf, size_t stride, size_t size, uint32 flag, void* buffer) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
            LEASSERT(device);

            D3D12_RESOURCE_DESC resourceDesc;
            resourceDesc.Width = (UINT64)(stride * size);
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
            resourceDesc.Alignment = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;

            D3D12_HEAP_PROPERTIES heapProps;
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }

            if (buffer) {
                D3D12_RANGE range = { 0, 0 };
                void* data = nullptr;
                if (SUCCEEDED(mResource->Map(0, &range, &data))) {
                    ::memcpy(data, buffer, stride * size);
                    mResource->Unmap(0, nullptr);
                }
            }
        }
        void Dispose() override
        {
            if (mResource)
                mResource->Release();
            mResource = nullptr;
        }
        void* GetHandle() override
        {
            return mResource;
        }
        void* GetResource() const override { return (void*)mResource; }
    private:
        ID3D12Resource      *mResource = nullptr;
    };
}