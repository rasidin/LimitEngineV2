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
@author minseob (https://github.com/rasidin)
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
        mSize = size;
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
    }

    void Update(void* data) override
    {
        if (mResource && data) {
            D3D12_RANGE range = { 0, 0 };
            void* mappeddata = nullptr;
            if (SUCCEEDED(mResource->Map(0, &range, &mappeddata))) {
                ::memcpy(mappeddata, data, mSize);
                mResource->Unmap(0, nullptr);
            }
        }
    }

    void* GetResource() const override { return static_cast<void*>(mResource); }
    void* GetConstantBufferView() const override { return (void*)&mConstantBufferView; }
private:
    ID3D12Resource* mResource = nullptr;
    size_t mSize = 0u;
    uint64 mGPUVirtualAddress = 0u;
    D3D12_CPU_DESCRIPTOR_HANDLE mConstantBufferView;
};
} // namespace LimitEngine