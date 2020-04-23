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

namespace LimitEngine {
struct PostProcessContext
{
    PooledRenderTarget SceneColor;
    PooledDepthStencil SceneDepth;
};

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
    void ResolveFinalResult(const PostProcessContext &Context);

private:
    Shader *mResolveShader = nullptr;
};
#define LE_PostProcessManager PostProcessManager::GetSingleton()
}