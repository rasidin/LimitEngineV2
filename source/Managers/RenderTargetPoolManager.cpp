/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  RenderTargetPoolManager.cpp
 @brief RenderTargetPool Manager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Managers/RenderTargetPoolManager.h"
#include "Managers/DrawManager.h"

#include "Renderer/Texture.h"

namespace LimitEngine {
RenderTargetPoolManager* RenderTargetPoolManager::mInstance = nullptr;
PooledRenderTarget::PooledRenderTarget(const PooledRenderTarget &RenderTarget)
: mTexture(RenderTarget.mTexture), mDesc(RenderTarget.mDesc)
{
    if (mTexture)
        mTexture->AddReferenceCounter();
}
void PooledRenderTarget::Release()
{
    if (mTexture) {
        LEASSERT(mTexture->GetReferenceCounter() > 0);
        if (mTexture->SubReferenceCounter() == 0) {
            mTexture->AddReferenceCounter();
            RenderTargetDesc captureddesc = mDesc;
            Texture* capturedtexture = mTexture;
            LE_DrawManager.AddRendererTaskLambda([captureddesc, capturedtexture]() {
                capturedtexture->SubReferenceCounter();
                LEASSERT(capturedtexture->GetReferenceCounter() == 0);
                LE_RenderTargetPoolManager.ReleaseRenderTarget(captureddesc, capturedtexture);
            });
        }
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
PooledRenderTarget RenderTargetPoolManager::GetRenderTarget(const RenderTargetDesc &InDesc, const char *InDebugName/* = nullptr*/)
{
    return GetRenderTarget(InDesc.Size, InDesc.Depth, InDesc.Format, InDebugName);
}
PooledRenderTarget RenderTargetPoolManager::GetRenderTarget(const LEMath::IntSize &Size, uint32 Depth, const RendererFlag::BufferFormat &Format, const char *InDebugName/* = nullptr*/)
{
    Mutex::ScopedLock lock(mBucketMutex);

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
        if (InDebugName)
            FoundRenderTarget->SetDebugName(InDebugName);
        return PooledRenderTarget(FoundRenderTarget, Desc);
    } else {
        Texture *newRenderTarget = new Texture();
        newRenderTarget->CreateRenderTarget(Size, Format);
        if (InDebugName)
            newRenderTarget->SetDebugName(InDebugName);
        return PooledRenderTarget(newRenderTarget, Desc);
    }
    return PooledRenderTarget(nullptr, Desc);
}
void RenderTargetPoolManager::ReleaseRenderTarget(const RenderTargetDesc& desc, Texture* texture)
{
    Mutex::ScopedLock lock(mBucketMutex);
    if (texture)
        mRTBuckets.Add(RTBucketType(desc, texture));
}
PooledDepthStencil RenderTargetPoolManager::GetDepthStencil(const LEMath::IntSize &Size, const RendererFlag::BufferFormat &Format, const char *InDebugName/* = nullptr*/)
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
        if (InDebugName)
            FoundDepthStencil->SetDebugName(InDebugName);
        return PooledDepthStencil(FoundDepthStencil, Desc);
    }
    else {
        Texture *newDepthStencil = new Texture();
        newDepthStencil->CreateDepthStencil(Size, Format);
        if (InDebugName)
            newDepthStencil->SetDebugName(InDebugName);
        return PooledDepthStencil(newDepthStencil, Desc);
    }
}
void RenderTargetPoolManager::ReleaseDepthStencil(PooledDepthStencil &DepthStencil)
{
    mDSBuckets.Add(DSBucketType(DepthStencil.mDesc, DepthStencil.mTexture));
}
}