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
void PooledRenderTarget::Release()
{
    if (mTexture)
    if (mTexture->SubReferenceCounter() == 0) {
        LE_RenderTargetPoolManager.ReleaseRenderTarget(*this);
    }
    mTexture = nullptr;
}
PooledDepthStencil::PooledDepthStencil(const PooledDepthStencil &DepthStencil)
    : mTexture(DepthStencil.mTexture), mDesc(DepthStencil.mDesc)
{
    mTexture->AddReferenceCounter();
}
void PooledDepthStencil::Release()
{
    if (mTexture)
    if (mTexture->SubReferenceCounter() == 0) {
        LE_RenderTargetPoolManager.ReleaseDepthStencil(*this);
    }
    mTexture = nullptr;
}
RenderTargetPoolManager::RenderTargetPoolManager()
    : SingletonRenderTargetPoolManager()
{}
RenderTargetPoolManager::~RenderTargetPoolManager()
{
    for (auto RTBucket : mRTBuckets) {
        delete RTBucket.value;
    }
    mRTBuckets.Clear();
    for (auto DSBucket : mDSBuckets) {
        delete DSBucket.value;
    }
    mDSBuckets.Clear();
}
PooledRenderTarget RenderTargetPoolManager::GetRenderTarget(const RenderTargetDesc &InDesc)
{
    return GetRenderTarget(InDesc.Size, InDesc.Depth, InDesc.Format);
}
PooledRenderTarget RenderTargetPoolManager::GetRenderTarget(const LEMath::IntSize Size, uint32 Depth, TEXTURE_COLOR_FORMAT Format)
{
    RenderTargetDesc Desc(Size, Depth, Format);
    Texture *FoundRenderTarget = nullptr;
    uint32 rtbidx = 0u;
    for (; rtbidx < mRTBuckets.count(); rtbidx++) {
        if (mRTBuckets[rtbidx].key == Desc) {
            FoundRenderTarget = mRTBuckets[rtbidx].value;
            break;
        }
    }
    if (FoundRenderTarget) {
        mRTBuckets.erase(rtbidx);
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
    if (RenderTarget.mTexture)
        mRTBuckets.Add(RTBucketType(RenderTarget.mDesc, RenderTarget.mTexture));
}
PooledDepthStencil RenderTargetPoolManager::GetDepthStencil(const LEMath::IntSize Size, TEXTURE_DEPTH_FORMAT Format)
{
    DepthStencilDesc Desc(Size, Format);
    Texture *FoundDepthStencil = nullptr;
    for (uint32 dsbidx = 0; dsbidx < mDSBuckets.count(); dsbidx++) {
        if (mDSBuckets[dsbidx].key == Desc) {
            FoundDepthStencil = mDSBuckets[dsbidx].value;
            break;
        }
    }
    if (FoundDepthStencil) {
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
    mDSBuckets.Add(DSBucketType(DepthStencil.mDesc, DepthStencil.mTexture));
}
}