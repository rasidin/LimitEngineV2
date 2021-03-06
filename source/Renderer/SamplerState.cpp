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
@file  SamplerState.cpp
@brief Sampler states cache
@author minseob (https://github.com/rasidin)
**********************************************************************/
#include "Renderer/SamplerState.h"

#if defined(USE_DX11)
#include "../Platform/DirectX11/SamplerStateImpl_DirectX11.inl"
#elif defined(USE_DX12)
#include "../Platform/DirectX12/SamplerStateImpl_DirectX12.inl"
#else
#error No implementation for TextureSampler
#endif

namespace LimitEngine {
VectorArray<SamplerStateRefPtr> SamplerState::sSamplerCache;
void SamplerState::TerminateCache() {
    for (SamplerStateRefPtr &CachedSampler : sSamplerCache) {
        CachedSampler.Release();
    }
    sSamplerCache.Clear();
}

SamplerState* SamplerState::Get(const SamplerStateDesc &Desc) {
    SamplerState *FoundSamplerState = nullptr;
    if (sSamplerCache.count())
    for (SamplerStateRefPtr &CachedSampler : sSamplerCache) {
        if (CachedSampler->GetDesc() == Desc) {
            FoundSamplerState = CachedSampler.Get();
            break;
        }
    }
    if (!FoundSamplerState) {
        FoundSamplerState = new SamplerState(Desc);
        FoundSamplerState->Create();
        sSamplerCache.Add(FoundSamplerState);
    }

    return FoundSamplerState;
}
SamplerState::SamplerState(const SamplerStateDesc &Desc)
    : mDesc(Desc)
    , mImpl(nullptr)
{
#if defined(USE_DX11)
    mImpl = new SamplerStateImpl_DirectX11();
#elif defined(USE_DX12)
    mImpl = new SamplerStateImpl_DirectX12();
#else
#error No implementation for TextureSampler
#endif
}
SamplerState::~SamplerState()
{
    if (mImpl)
        delete mImpl;
    mImpl = nullptr;
}
void SamplerState::Create()
{
    if (mImpl)
        mImpl->Create(mDesc);
}
}