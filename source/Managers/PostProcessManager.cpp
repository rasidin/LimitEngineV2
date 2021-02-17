/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  PostProcessManager.cpp
 @brief PostProcessManager Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#include "Managers/PostProcessManager.h"

#include "Managers/Draw2DManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/RenderTargetPoolManager.h"
#include "Renderer/Camera.h"
#include "Renderer/DrawCommand.h"

#include "PostProcessors/PostProcessAmbientOcclusion.h"
#include "PostProcessors/PostProcessTemporalAA.h"
#include "PostProcessors/PostProcessResolveFinalColor.h"

namespace LimitEngine {
PostProcessManager* PostProcessManager::mInstance = nullptr;
PostProcessManager::PostProcessManager()
    : SingletonPostProcessManager()
{
    //PostProcessors.Add(new PostProcessAmbientOcclusion());
    //PostProcessors.Add(new PostProcessTemporalAA());
    PostProcessors.Add(new PostProcessResolveFinalColor());
}
PostProcessManager::~PostProcessManager()
{
    for (auto *PostProcessor : PostProcessors)
    {
        delete PostProcessor;
    }
    PostProcessors.Clear();
}
void PostProcessManager::Init(const InitializeOptions &Options)
{
    for (auto *PostProcessor : PostProcessors)
    {
        InitializeOptions CopiedOptions = Options;
        LE_DrawManager.AddRendererTaskLambda([PostProcessor, CopiedOptions]() {
            PostProcessor->Init(CopiedOptions);
        });
    }
}
void PostProcessManager::Process()
{
    PostProcessContext Context;
    Context.RenderStateContext = LE_DrawManager.GetRenderStatePtr();
    Context.SceneColor = LE_SceneManager.GetSceneColor();
    Context.SceneDepth = LE_SceneManager.GetSceneDepth();
    Context.SceneNormal = LE_SceneManager.GetSceneNormal();
    Context.BlueNoiseTexture = LE_DrawManager.GetBlueNoiseTexture();
    Context.BlueNoiseContext = LE_DrawManager.GetBlueNoiseContext();
    Context.FrameIndexContext = LE_DrawManager.GetFrameIndexContext();

    VectorArray<PooledRenderTarget> RenderTargets;
    RenderTargets.Add(Context.SceneColor);
    for (auto *PostProcessor : PostProcessors)
    {
        PostProcessor->Process(Context, RenderTargets);
    }
}
}