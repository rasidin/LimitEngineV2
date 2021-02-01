/***********************************************************
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
-----------------------------------------------------------
@file  SamplerStateImpl_DirectX12.inl
@brief SamplerState (DirectX12)
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifndef LIMITENGINEV2_SAMPLERSTATEIMPL_DIRECTX12_INL_
#define LIMITENGINEV2_SAMPLERSTATEIMPL_DIRECTX12_INL_

#include <d3d12.h>

#include "Managers/DrawManager.h"

#include "PrivateDefinitions_DirectX12.h"

namespace LimitEngine {
D3D12_SAMPLER_DESC ConvertSamplerStateDescToD3D12SamplerDesc(const SamplerStateDesc &desc)
{
    D3D12_SAMPLER_DESC output;

    static constexpr D3D12_FILTER SamplerStateFilterToD3D12Filter[] = {
        D3D12_FILTER_MIN_MAG_MIP_POINT,                 // MIN_MAG_MIP_POINT
        D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,          // MIN_MAG_POINT_MIP_LINEAR
        D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,    // MIN_POINT_MAG_LINEAR_MIP_POINT
        D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,          // MIN_POINT_MAG_MIP_LINEAR
        D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT,          // MIN_LINEAR_MAG_MIP_POINT
        D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,   // MIN_LINEAR_MAG_POINT_MIP_LINEAR
        D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,          // MIN_MAG_LINEAR_MIP_POINT
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,                // MIN_MAG_MIP_LINEAR
        D3D12_FILTER_ANISOTROPIC                        // ANISOTROPIC
    };

    static constexpr D3D12_TEXTURE_ADDRESS_MODE SamplerStateAddressModeToD3D12TextureAddressMode[] = {
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,        // Wrap,
        D3D12_TEXTURE_ADDRESS_MODE_MIRROR,      // Mirror,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,       // Clamp,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,      // Border,
        D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE  // Mirror_Once
    };

    output.Filter = SamplerStateFilterToD3D12Filter[static_cast<int>(desc.Filter)];
    output.AddressU = SamplerStateAddressModeToD3D12TextureAddressMode[static_cast<int>(desc.AddressU)];
    output.AddressV = SamplerStateAddressModeToD3D12TextureAddressMode[static_cast<int>(desc.AddressV)];
    output.AddressW = SamplerStateAddressModeToD3D12TextureAddressMode[static_cast<int>(desc.AddressW)];
    output.MipLODBias = desc.MipLODBias;
    output.MaxAnisotropy = desc.MaxAnisotropy;
    output.ComparisonFunc = TestFlagsToD3D12ComparisonFunc[static_cast<int>(desc.ComparisonFunc)];
    output.BorderColor[0] = desc.BorderColor.X();
    output.BorderColor[1] = desc.BorderColor.Y();
    output.BorderColor[2] = desc.BorderColor.Z();
    output.BorderColor[3] = desc.BorderColor.W();
    output.MinLOD = desc.MinLOD;
    output.MaxLOD = desc.MaxLOD;

    return output;
}
class SamplerStateImpl_DirectX12 : public SamplerStateImpl
{
public:
    SamplerStateImpl_DirectX12() {}
    virtual ~SamplerStateImpl_DirectX12() {}

    void Create(const SamplerStateDesc& Desc)
    {
        mHandle = {};
        mHandle.ptr = reinterpret_cast<SIZE_T>(LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)));
        D3D12_SAMPLER_DESC d3dsamplerdesc = ConvertSamplerStateDescToD3D12SamplerDesc(Desc);
        ((ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle())->CreateSampler(
            &d3dsamplerdesc, mHandle
        );
    }

    void* GetHandle() { return &mHandle; }

private:
    D3D12_CPU_DESCRIPTOR_HANDLE mHandle;
};
}

#endif // LIMITENGINEV2_SAMPLERSTATEIMPL_DIRECTX12_INL_