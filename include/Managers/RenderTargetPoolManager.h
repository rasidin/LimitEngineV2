/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  RenderTargetPoolManager.h
 @brief RenderTargetPool Manager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LEIntVector2.h>

#include "Core/Singleton.h"
#include "Containers/MapArray.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
class RenderTargetPoolManager;
typedef Singleton<RenderTargetPoolManager, LimitEngineMemoryCategory_Graphics> SingletonRenderTaregtPoolManager;
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
    ~PooledRenderTarget();

    Texture* Get() const {
        return mTexture;
    }

    PooledRenderTarget& operator = (const PooledRenderTarget &In)
    {
        mTexture = In.mTexture;
        mDesc = In.mDesc;
        mTexture->AddReferenceCounter();
        return *this;
    }

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
    ~PooledDepthStencil();

    Texture* Get() const {
        return mTexture;
    }

    PooledDepthStencil& operator = (const PooledDepthStencil &In)
    {
        mTexture = In.mTexture;
        mDesc = In.mDesc;
        mTexture->AddReferenceCounter();
        return *this;
    }

    friend RenderTargetPoolManager;
};
class RenderTargetPoolManager : public SingletonRenderTaregtPoolManager
{
public:
    RenderTargetPoolManager();
    virtual ~RenderTargetPoolManager();

    PooledRenderTarget GetRenderTarget(const LEMath::IntSize Size, uint32 Depth, TEXTURE_COLOR_FORMAT Format);
    void ReleaseRenderTarget(PooledRenderTarget &RenderTarget);

    PooledDepthStencil GetDepthStencil(const LEMath::IntSize Size, TEXTURE_DEPTH_FORMAT Format);
    void ReleaseDepthStencil(PooledDepthStencil &DepthStencil);

private:
    MapArray<RenderTargetDesc, Texture*> mRTBuckets;
    MapArray<DepthStencilDesc, Texture*> mDSBuckets;
};
#define LE_RenderTargetPoolManager RenderTargetPoolManager::GetSingleton()
}