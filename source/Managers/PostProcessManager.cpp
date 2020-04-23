/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  PostProcessManager.cpp
 @brief PostProcessManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Managers/PostProcessManager.h"

#include "Managers/Draw2DManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/Camera.h"
#include "Renderer/DrawCommand.h"

namespace LimitEngine {
PostProcessManager* PostProcessManager::mInstance = nullptr;
PostProcessManager::PostProcessManager()
    : SingletonPostProcessManager()
{
}
PostProcessManager::~PostProcessManager()
{
    if (mResolveShader) {
        delete mResolveShader;
    }
}
void PostProcessManager::Init(const InitializeOptions &Options)
{
    switch (Options.OutputColorSpace) 
    {
    case InitializeOptions::ColorSpace::sRGB:
        mResolveShader = LE_ShaderManager.GetShader("ResolveSceneColorSRGB");
        break;
    case InitializeOptions::ColorSpace::PQ:
        mResolveShader = LE_ShaderManager.GetShader("ResolveSceneColorSRGB"); // Temporary
        break;
    }
}
void PostProcessManager::Process()
{
    PostProcessContext Context;
    Context.SceneColor = LE_SceneManager.GetSceneColor();
    Context.SceneDepth = LE_SceneManager.GetSceneDepth();
    // Resolve final scene color
    ResolveFinalResult(Context);
}
void PostProcessManager::ResolveFinalResult(const PostProcessContext &Context)
{
    if (!mResolveShader) return;
    DrawCommand::SetRenderTarget(0, nullptr, nullptr);
    DrawCommand::BindTexture(0, Context.SceneColor.Get());

    DrawCommand::SetDepthFunc(RendererFlag::TestFlags::ALWAYS);
    DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::SOURCE);

    if (Camera *MainCamera = LE_SceneManager.GetCamera()) {
        int EVOffsetParam = mResolveShader->GetUniformLocation("EVOffset");
        if (EVOffsetParam >= 0) {
            DrawCommand::SetShaderUniformFloat1(mResolveShader, EVOffsetParam, MainCamera->GetExposure());
        }
    }

    Vertex2D *buffer = LE_Draw2DManager.GetVertexBuffer2D(6);
    buffer[0].SetPosition(LEMath::FloatVector3(0.0f, 0.0f, 0.0f));
    buffer[0].SetColor(0xffffffff);
    buffer[0].SetTexcoord(LEMath::FloatVector2(0.0f, 0.0f));
    buffer[1].SetPosition(LEMath::FloatVector3(0.0f, 1.0f, 0.0f));
    buffer[1].SetColor(0xffffffff);
    buffer[1].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
    buffer[2].SetPosition(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
    buffer[2].SetColor(0xffffffff);
    buffer[2].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
    buffer[3].SetPosition(LEMath::FloatVector3(0.0f, 1.0f, 0.0f));
    buffer[3].SetColor(0xffffffff);
    buffer[3].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
    buffer[4].SetPosition(LEMath::FloatVector3(1.0f, 1.0f, 0.0f));
    buffer[4].SetColor(0xffffffff);
    buffer[4].SetTexcoord(LEMath::FloatVector2(1.0f, 1.0f));
    buffer[5].SetPosition(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
    buffer[5].SetColor(0xffffffff);
    buffer[5].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
    LE_Draw2DManager.FlushDraw2D(RendererFlag::PrimitiveTypes::TRIANGLELIST, mResolveShader);
}
}