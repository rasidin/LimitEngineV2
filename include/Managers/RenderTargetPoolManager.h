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
@file  RenderTargetPoolManager.h
@brief RenderTargetPool Manager Class
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#pragma once

#include <LEIntVector2.h>

#include "Core/Singleton.h"
#include "Core/Mutex.h"
#include "Containers/MapArray.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
class RenderTargetPoolManager;
typedef Singleton<RenderTargetPoolManager, LimitEngineMemoryCategory_Graphics> SingletonRenderTargetPoolManager;
struct RenderTargetDesc
{
    LEMath::IntSize Size;
    uint32 Depth;
    TEXTURE_COLOR_FORMAT Format;

    RenderTargetDesc()
        : Size(0, 0)
        , Depth(0)
    {}
    RenderTargetDesc(const RenderTargetDesc &In)
    {
        Size = In.Size;
        Depth = In.Depth;
        Format = In.Format;
    }
    RenderTargetDesc(const LEMath::IntSize &InSize, uint32 InDepth, TEXTURE_COLOR_FORMAT InFormat)
        : Size(InSize), Depth(InDepth), Format(InFormat)
    {}

    bool operator == (const RenderTargetDesc &Desc) const {
        return Size == Desc.Size
            && Depth == Desc.Depth
            && Format == Desc.Format;
    }
};
struct DepthStencilDesc
{
    LEMath::IntSize Size;
    TEXTURE_DEPTH_FORMAT Format;

    DepthStencilDesc()
        : Size(0, 0)
    {}
    DepthStencilDesc(const DepthStencilDesc &In)
    {
        Size = In.Size;
        Format = In.Format;
    }
    DepthStencilDesc(const LEMath::IntSize &InSize, TEXTURE_DEPTH_FORMAT InFormat)
        : Size(InSize), Format(InFormat)
    {}

    bool operator == (const DepthStencilDesc &Desc) const {
        return Size == Desc.Size && Format == Desc.Format;
    }
};
class PooledRenderTarget
{
    Texture *mTexture = nullptr;
    RenderTargetDesc mDesc;

    PooledRenderTarget(Texture *InTexture, const RenderTargetDesc &Desc) : mTexture(InTexture), mDesc(Desc) {
        mTexture->AddReferenceCounter();
    }
public:
    PooledRenderTarget() : mTexture(nullptr) {}
    PooledRenderTarget(const PooledRenderTarget &RenderTarget);
    ~PooledRenderTarget() { Release(); }

    void Release();

    Texture* Get() const {
        return mTexture;
    }

    PooledRenderTarget& operator = (const PooledRenderTarget &In)
    {
        Release();

        if (mTexture = In.mTexture) {
            mDesc = In.mDesc;
            mTexture->AddReferenceCounter();
        }
        return *this;
    }

    const RenderTargetDesc& GetDesc() const { return mDesc; }

    friend RenderTargetPoolManager;
};
class PooledDepthStencil
{
    Texture *mTexture = nullptr;
    DepthStencilDesc mDesc;

    PooledDepthStencil(Texture *InTexture, const DepthStencilDesc &Desc) : mTexture(InTexture), mDesc(Desc) {
        mTexture->AddReferenceCounter();
    }
public:
    PooledDepthStencil() : mTexture(nullptr) {}
    PooledDepthStencil(const PooledDepthStencil &DepthStencil);
    ~PooledDepthStencil() { Release(); }

    void Release();

    Texture* Get() const {
        return mTexture;
    }

    PooledDepthStencil& operator = (const PooledDepthStencil &In)
    {
        Release();

        mTexture = In.mTexture;
        mDesc = In.mDesc;
        mTexture->AddReferenceCounter();
        return *this;
    }

    friend RenderTargetPoolManager;
};
class RenderTargetPoolManager : public SingletonRenderTargetPoolManager
{
public:
    RenderTargetPoolManager();
    virtual ~RenderTargetPoolManager();

    PooledRenderTarget GetRenderTarget(const RenderTargetDesc &InDesc, const char *InDebugName = nullptr);
    PooledRenderTarget GetRenderTarget(const LEMath::IntSize Size, uint32 Depth, TEXTURE_COLOR_FORMAT Format, const char *InDebugName = nullptr);
    void ReleaseRenderTarget(PooledRenderTarget &RenderTarget);

    PooledDepthStencil GetDepthStencil(const LEMath::IntSize Size, TEXTURE_DEPTH_FORMAT Format, const char *InDebugName = nullptr);
    void ReleaseDepthStencil(PooledDepthStencil &DepthStencil);

private:
    Mutex mBucketMutex;

    typedef Pair<RenderTargetDesc, Texture*> RTBucketType;
    VectorArray<RTBucketType> mRTBuckets;
    typedef Pair<DepthStencilDesc, Texture*> DSBucketType;
    VectorArray<DSBucketType> mDSBuckets;
};
#define LE_RenderTargetPoolManager RenderTargetPoolManager::GetSingleton()
}