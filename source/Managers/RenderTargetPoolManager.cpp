/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  RenderTargetPoolManager.cpp
 @brief RenderTargetPool Manager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Managers/RenderTargetPoolManager.h"

#include "Renderer/Texture.h"

namespace LimitEngine {
RenderTargetPoolManager* RenderTargetPoolManager::mInstance = nullptr;
PooledRenderTarget::PooledRenderTarget(const PooledRenderTarget &RenderTarget)
: mTexture(RenderTarget.mTexture), mDesc(RenderTarget.mDesc)
{
    mTexture->AddReferenceCounter();
}
PooledRenderTarget::~PooledRenderTarget()
{
    if (mTexture->SubReferenceCounter() == 0) {
        LE_RenderTargetPoolManager.ReleaseRenderTarget(*this);
    }
}
PooledDepthStencil::PooledDepthStencil(const PooledDepthStencil &DepthStencil)
    : mTexture(DepthStencil.mTexture), mDesc(DepthStencil.mDesc)
{
    mTexture->AddReferenceCounter();
}
PooledDepthStencil::~PooledDepthStencil()
{
    if (mTexture->SubReferenceCounter() == 0) {
        LE_RenderTargetPoolManager.ReleaseDepthStencil(*this);
    }
}
RenderTargetPoolManager::RenderTargetPoolManager()
    : SingletonRenderTaregtPoolManager()
{}
RenderTargetPoolManager::~RenderTargetPoolManager()
{}
PooledRenderTarget RenderTargetPoolManager::GetRenderTarget(const LEMath::IntSize Size, uint32 Depth, TEXTURE_COLOR_FORMAT Format)
{
    RenderTargetDesc Desc(Size, Depth, Format);
    if (Texture *FoundRenderTarget = mRTBuckets.Find(Desc)) {
        return PooledRenderTarget(FoundRenderTarget, Desc);
    } else {
        Texture *newRenderTarget = new Texture();
        newRenderTarget->CreateRenderTarget(Size, Format);
        return PooledRenderTarget(newRenderTarget, Desc);
    }
    return PooledRenderTarget(nullptr, Desc);
}
void RenderTargetPoolManager::ReleaseRenderTarget(PooledRenderTarget &RenderTarget)
{
    mRTBuckets.Add(RenderTarget.mDesc, RenderTarget.mTexture);
}
PooledDepthStencil RenderTargetPoolManager::GetDepthStencil(const LEMath::IntSize Size, TEXTURE_DEPTH_FORMAT Format)
{
    DepthStencilDesc Desc(Size, Format);
    if (Texture *FoundDepthStencil = mDSBuckets.Find(Desc)) {
        return PooledDepthStencil(FoundDepthStencil, Desc);
    }
    else {
        Texture *newDepthStencil = new Texture();
        newDepthStencil->CreateDepthStencil(Size, Format);
        return PooledDepthStencil(newDepthStencil, Desc);
    }
}
void RenderTargetPoolManager::ReleaseDepthStencil(PooledDepthStencil &DepthStencil)
{
    mDSBuckets.Add(DepthStencil.mDesc, DepthStencil.mTexture);
}
}