/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  PostProcessManager.h
 @brief PostProcessManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <leRenderer>

#include "InitializeOptions.h"
#include "Core/Singleton.h"
#include "Managers/RenderTargetPoolManager.h"
#include "PostProcessors/PostProcessor.h"

namespace LimitEngine {
class PostProcessManager;
typedef Singleton<PostProcessManager, LimitEngineMemoryCategory_Graphics> SingletonPostProcessManager;
class PostProcessManager : public SingletonPostProcessManager
{
public:
    PostProcessManager();
    virtual ~PostProcessManager();

    void Init(const InitializeOptions &Options);

    void Process();

private:
    VectorArray<PostProcessor*> PostProcessors;

    ShaderRefPtr mAmbientOcclusionShader;
    ShaderRefPtr mAmbientOcclusionBlurShader;
    ShaderRefPtr mTemporalAAShader;
    ShaderRefPtr mResolveShader;

    int mAmbientOcclusionParameter_SceneDepth;

    PooledRenderTarget mHistorySceneColor;
};
#define LE_PostProcessManager PostProcessManager::GetSingleton()
}