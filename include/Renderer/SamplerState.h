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
@file  SamplerState.h
@brief Sampler states cache
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#pragma once

#include <LERenderer>
#include <LEFloatVector4.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/ReferenceCountedPointer.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
enum class SamplerStateFilter
{
    MIN_MAG_MIP_POINT = 0,
    MIN_MAG_POINT_MIP_LINEAR,
    MIN_POINT_MAG_LINEAR_MIP_POINT,
    MIN_POINT_MAG_MIP_LINEAR,
    MIN_LINEAR_MAG_MIP_POINT,
    MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    MIN_MAG_LINEAR_MIP_POINT,
    MIN_MAG_MIP_LINEAR,
    ANISOTROPIC
};
enum class SamplerStateAddressMode
{
    Wrap = 0,
    Mirror,
    Clamp,
    Border,
    Mirror_Once
};
struct SamplerStateDesc
{
    SamplerStateFilter Filter;
    SamplerStateAddressMode AddressU;
    SamplerStateAddressMode AddressV;
    SamplerStateAddressMode AddressW;
    float MipLODBias;
    uint32 MaxAnisotropy;
    RendererFlag::TestFlags ComparisonFunc;
    LEMath::FloatColorRGBA BorderColor;
    float MinLOD;
    float MaxLOD;
    SamplerStateDesc(
        SamplerStateFilter              InFilter        = SamplerStateFilter::MIN_MAG_MIP_LINEAR,
        SamplerStateAddressMode         InAddressU      = SamplerStateAddressMode::Wrap,
        SamplerStateAddressMode         InAddressV      = SamplerStateAddressMode::Wrap,
        SamplerStateAddressMode         InAddressW      = SamplerStateAddressMode::Wrap,
        float                           InMipLODBias    = 0.0f,
        uint32                          InMaxAnisotropy = 0,
        RendererFlag::TestFlags         InCompFunc      = RendererFlag::TestFlags::ALWAYS,
        const LEMath::FloatColorRGBA&   InBorderColor   = LEMath::FloatColorRGBA::Zero,
        float                           InMinLOD        = 0.0f,
        float                           InMaxLOD        = 255.0f
    )
        : Filter(InFilter)
        , AddressU(InAddressU)
        , AddressV(InAddressV)
        , AddressW(InAddressW)
        , MipLODBias(InMipLODBias)
        , MaxAnisotropy(InMaxAnisotropy)
        , ComparisonFunc(InCompFunc)
        , BorderColor(InBorderColor)
        , MinLOD(InMinLOD)
        , MaxLOD(InMaxLOD)
    {}
    bool operator == (const SamplerStateDesc &d) const {
        return Filter == d.Filter &&
            AddressU == d.AddressU &&
            AddressV == d.AddressV &&
            AddressW == d.AddressW &&
            MipLODBias == d.MipLODBias &&
            MaxAnisotropy == d.MaxAnisotropy &&
            ComparisonFunc == d.ComparisonFunc &&
            BorderColor == d.BorderColor &&
            MinLOD == d.MinLOD &&
            MaxLOD == d.MaxLOD;
    }
};
class SamplerStateImpl : public Object<LimitEngineMemoryCategory::Graphics>
{public:
    SamplerStateImpl() {}
    virtual ~SamplerStateImpl() {}

    virtual void Create(const SamplerStateDesc &Desc) = 0;
    virtual void* GetHandle() = 0;
};
class SamplerState : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics> {
    static VectorArray<SamplerStateRefPtr> sSamplerCache;
public:
    virtual ~SamplerState();

    static void TerminateCache();
    static SamplerState* Get(const SamplerStateDesc &Desc);

    void Create();
    const SamplerStateDesc& GetDesc() const { return mDesc; }
    void* GetHandle() { return (mImpl) ? mImpl->GetHandle() : nullptr; }

private:
    explicit SamplerState(const SamplerStateDesc &Desc);

private:
    SamplerStateDesc  mDesc;
    SamplerStateImpl *mImpl;
};
}