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
@file  SamplerStateImpl_DirectX11.inl
@brief SamplerState (DirectX11)
@author minseob (https://github.com/rasidin)
***********************************************************/
#pragma once
#include <d3d11.h>

#include "Core/Debug.h"

#include <Managers/DrawManager.h>

namespace LimitEngine {
D3D11_FILTER ConvertFilterToD3D11(SamplerStateFilter Filter)
{
    switch (Filter)
    {
    case SamplerStateFilter::MIN_MAG_MIP_POINT:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
    case SamplerStateFilter::MIN_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case SamplerStateFilter::MIN_POINT_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case SamplerStateFilter::MIN_POINT_MAG_MIP_LINEAR:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    case SamplerStateFilter::MIN_LINEAR_MAG_MIP_POINT:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case SamplerStateFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case SamplerStateFilter::MIN_MAG_LINEAR_MIP_POINT:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case SamplerStateFilter::MIN_MAG_MIP_LINEAR:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    case SamplerStateFilter::ANISOTROPIC:
        return D3D11_FILTER_ANISOTROPIC;
    default:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
    }
}
D3D11_TEXTURE_ADDRESS_MODE ConvertTextureAddressModeToD3D11(SamplerStateAddressMode AddressMode)
{
    switch (AddressMode)
    {
    case SamplerStateAddressMode::Wrap:
        return D3D11_TEXTURE_ADDRESS_WRAP;
    case SamplerStateAddressMode::Mirror:
        return D3D11_TEXTURE_ADDRESS_MIRROR;
    case SamplerStateAddressMode::Clamp:
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    case SamplerStateAddressMode::Border:
        return D3D11_TEXTURE_ADDRESS_BORDER;
    case SamplerStateAddressMode::Mirror_Once:
        return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    default:
        return D3D11_TEXTURE_ADDRESS_WRAP;
    }
}
D3D11_COMPARISON_FUNC ConvertTestFlagsToD3D11(RendererFlag::TestFlags TestFlag)
{
    switch (TestFlag)
    {
    case RendererFlag::TestFlags::ALWAYS:
        return D3D11_COMPARISON_ALWAYS;
    case RendererFlag::TestFlags::EQUAL:
        return D3D11_COMPARISON_EQUAL;
    case RendererFlag::TestFlags::GEQUAL:
        return D3D11_COMPARISON_GREATER_EQUAL;
    case RendererFlag::TestFlags::GREATER:
        return D3D11_COMPARISON_GREATER;
    case RendererFlag::TestFlags::LEQUAL:
        return D3D11_COMPARISON_LESS_EQUAL;
    case RendererFlag::TestFlags::LESS:
        return D3D11_COMPARISON_LESS;
    case RendererFlag::TestFlags::NEVER:
        return D3D11_COMPARISON_NEVER;
    case RendererFlag::TestFlags::NOTEQUAL:
        return D3D11_COMPARISON_NOT_EQUAL;
    default:
        return D3D11_COMPARISON_NEVER;
    }
}
class SamplerStateImpl_DirectX11 : public SamplerStateImpl
{
public:
    SamplerStateImpl_DirectX11() {}
    virtual ~SamplerStateImpl_DirectX11() {
        if (mSampler) {
            mSampler->Release();
        }
        mSampler = nullptr;
    }

    void Create(const SamplerStateDesc &Desc) 
    {
        ID3D11Device *device = (ID3D11Device*)LE_DrawManager.GetDeviceHandle();
        LEASSERT(device);

        D3D11_SAMPLER_DESC samplerDesc;
        ::memset(&samplerDesc, 0, sizeof(samplerDesc));
        samplerDesc.Filter = ConvertFilterToD3D11(Desc.Filter);
        samplerDesc.AddressU = ConvertTextureAddressModeToD3D11(Desc.AddressU);
        samplerDesc.AddressV = ConvertTextureAddressModeToD3D11(Desc.AddressV);
        samplerDesc.AddressW = ConvertTextureAddressModeToD3D11(Desc.AddressW);
        samplerDesc.ComparisonFunc = ConvertTestFlagsToD3D11(Desc.ComparisonFunc);
        samplerDesc.BorderColor[0] = Desc.BorderColor.X();
        samplerDesc.BorderColor[1] = Desc.BorderColor.Y();
        samplerDesc.BorderColor[2] = Desc.BorderColor.Z();
        samplerDesc.BorderColor[3] = Desc.BorderColor.W();
        samplerDesc.MaxAnisotropy = Desc.MaxAnisotropy;
        samplerDesc.MaxLOD = Desc.MaxLOD;
        samplerDesc.MinLOD = Desc.MinLOD;
        samplerDesc.MipLODBias = Desc.MipLODBias;

        if (device->CreateSamplerState(&samplerDesc, &mSampler) != S_OK) {
            Debug::Error("[SamplerStateImpl_DirectX11] Failed to create sampler state");
            return;
        }
    }

    void* GetHandle() { return mSampler; }
private:
    ID3D11SamplerState      *mSampler;
};
}