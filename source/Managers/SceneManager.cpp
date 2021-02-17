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
 @file  SceneManager.cpp
 @brief SceneManager Class
 @author minseob (leeminseob@outlook.com)
 *********************************************************************/
#include "Managers/SceneManager.h"

#include <LEFloatVector4.h>

#include "Core/Debug.h"
#include "Core/TaskPriority.h"
#include "Core/TextParser.h"
#include "PostProcessors/PostProcessAmbientOcclusion.h"
#include "Managers/DrawManager.h"
#include "Managers/Draw2DManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/Camera.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Font.h"
#include "Renderer/Model.h"
#include "Renderer/ModelInstance.h"
#include "Renderer/Light.h"
#include "Renderer/LightIBL.h"
#include "Renderer/Texture.h"
#include "Renderer/PipelineState.h"
#include "Renderer/SamplerState.h"

#include "Shaders/Draw2D.ps.h"
#include "Shaders/DrawFullscreen.ps.h"

namespace LimitEngine {
class SceneUpdateTask_AddModel : public SceneManager::SceneUpdateTask
{
    ModelRefPtr mModel;
    uint32 mModelInstanceID;

public:
    SceneUpdateTask_AddModel(Model *InModel, uint32 ID) : mModel(InModel), mModelInstanceID(ID) {}
    void Run(SceneManager *Manager) override
    {
        Manager->AddModel_UpdateTask(mModel.Get(), mModelInstanceID);
    }
};

class SceneUpdateTask_UpdateModelTransform : public SceneManager::SceneUpdateTask
{
    uint32 mInstanceID;
    Transform mTransform;
public:
    SceneUpdateTask_UpdateModelTransform(uint32 InstanceID, const Transform &InTransform)
        : mInstanceID(InstanceID)
        , mTransform(InTransform)
    {}
    void Run(SceneManager *Manager) override
    {
        ModelInstRefPtr FoundModel = Manager->findModelInstance(mInstanceID);
        if (FoundModel.IsValid()) {
            FoundModel->SetTransform(mTransform);
        }
    }
};

class SceneUpdateTask_AddLight : public SceneManager::SceneUpdateTask
{
    LightRefPtr mLight;

public:
    SceneUpdateTask_AddLight(Light *InLight) : mLight(InLight) {}
    void Run(SceneManager *Manager) override
    {
        Manager->AddLight_UpdateTask(mLight.Get());
    }
};

class SceneUpdateTask_SetCamera : public SceneManager::SceneUpdateTask
{
    CameraRefPtr mCamera;

public:
    SceneUpdateTask_SetCamera(Camera *InCamera) : mCamera(InCamera) {}
    void Run(SceneManager *Manager) override
    {
        Manager->SetCamera_UpdateTask(mCamera.Get());
    }
};

#ifdef WIN32
SceneManager* SingletonSceneManager::mInstance = NULL;
#endif
SceneManager::SceneManager()
    : mCamera(new Camera())
    , mEnvironmentLight(nullptr)
    , mCurrentInstanceID(1u)
    , mBackgroundType(BackgroundImageType::None)
    , mAmbientOcclusion(nullptr)
    , mBackgroundColorCovertParameter(1.0f, 1.0f, 1.0f, 0.0f)
{
    mAmbientOcclusion = new PostProcessAmbientOcclusion();
}

SceneManager::~SceneManager()
{
    for (uint32 RTIndex = 0; RTIndex < PendingDeleteRenderTargetCount; RTIndex++) {
        if (mPendingReleaseRenderTargets[RTIndex].Get())
            mPendingReleaseRenderTargets[RTIndex].Release();
    }
    delete mAmbientOcclusion;
}

void SceneManager::Init(const InitializeOptions &InitOptions)
{
    RendererFlag::BufferFormat SceneColorFormat = (InitOptions.SceneColorSpace == InitializeOptions::ColorSpace::Linear) ? RendererFlag::BufferFormat::R16G16B16A16_Float : RendererFlag::BufferFormat::R8G8B8A8_UNorm;
    RendererFlag::BufferFormat SceneNormalFormat = RendererFlag::BufferFormat::R16G16B16A16_Float;
    mSceneColor = LE_RenderTargetPoolManager.GetRenderTarget(InitOptions.Resolution, 1, SceneColorFormat, "SceneColor");
    mSceneNormal = LE_RenderTargetPoolManager.GetRenderTarget(InitOptions.Resolution, 1, SceneNormalFormat, "SceneNormal");
    mSceneDepth = LE_RenderTargetPoolManager.GetDepthStencil(InitOptions.Resolution, RendererFlag::BufferFormat::D32_Float, "SceneDepth");
}

void SceneManager::PostInit(const InitializeOptions &InitOptions)
{
    mBackgroundShaders.Resize(static_cast<uint32>(BackgroundImageType::Num));
    mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Fullscreen)] = LE_ShaderManager.GetShader<DrawFullscreen_PS>();
    mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Longlat)] = LE_ShaderManager.GetShader<Draw2D_PS>();
    
    mBackgroundPipelineStates.Resize(static_cast<uint32>(BackgroundImageType::Num));
    for (uint32 bpsidx = 0; bpsidx < static_cast<uint32>(BackgroundImageType::Num); bpsidx++) {
        mBackgroundPipelineStates[bpsidx] = new PipelineState();
        
        PipelineStateDescriptor psdesc;
        psdesc.SetRenderTargetBlendEnabled(0, false);
        psdesc.SetRenderTargetFormat(0, mSceneColor.Get());
        psdesc.SetDepthEnabled(true);
        psdesc.SetDepthFunc(RendererFlag::TestFlags::Always);
        psdesc.SetStencilEnabled(false);
        psdesc.SetDepthWriteMask(RendererFlag::DepthWriteMask::All);
        psdesc.Shaders[static_cast<int>(Shader::Type::Pixel)] = mBackgroundShaders[bpsidx];

        LE_Draw2DManager.BuildPipelineState(psdesc);
        psdesc.Finalize();

        mBackgroundPipelineStates[bpsidx]->Init(psdesc);
    }

    //mBackgroundConstantBuffers.Resize(static_cast<uint32>(BackgroundImageType::Num));
    //for (uint32 cbIndex = 0, cbCount = static_cast<uint32>(BackgroundImageType::Num); cbIndex < cbCount; cbIndex++) {
    //    if (mBackgroundShaders[cbIndex].IsValid()) {
    //        mBackgroundConstantBuffers[cbIndex] = new ConstantBuffer();
    //        mBackgroundConstantBuffers[cbIndex]->Create(mBackgroundShaders[cbIndex].Get());
    //    }
    //}

    //mBackgroundColorCovertParameterIndex = mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Fullscreen)]->GetUniformLocation("ColorConvertParameters");

    //PostProcessAmbientOcclusion *CapturedAmbientOcclusion = mAmbientOcclusion;
    //LE_DrawManager.AddRendererTaskLambda([CapturedAmbientOcclusion, InitOptions]() {
    //    CapturedAmbientOcclusion->Init(InitOptions);
    //});
}

void SceneManager::SetBackgroundImage(const TextureRefPtr &BackgroundImage, BackgroundImageType Type)
{
    mBackgroundImage = BackgroundImage;
    mBackgroundType = Type;
}

void SceneManager::AddModel_UpdateTask(Model *InModel, uint32 InID)
{
    mModels.push_back(new ModelInstance(InID, InModel));
}

uint32 SceneManager::AddModel(const ModelRefPtr &model)
{
    Mutex::ScopedLock lock(mUpdateSceneMutex);
    mUpdateTasks.Add(new SceneUpdateTask_AddModel(model.Get(), mCurrentInstanceID++));
    return mCurrentInstanceID - 1;
}

void SceneManager::UpdateModelTransform(uint32 InstanceID, const Transform &InTransform)
{
    Mutex::ScopedLock lock(mUpdateSceneMutex);
    mUpdateTasks.Add(new SceneUpdateTask_UpdateModelTransform(InstanceID, InTransform));
}

void SceneManager::AddLight_UpdateTask(Light *InLight)
{
    mLights.Add(InLight);

    if (InLight->GetType() == Light::TYPE_IBL) {
        mEnvironmentLight = InLight;
    }

    mOnChangeEvent();
}

void SceneManager::AddLight(const LightRefPtr &light)
{
    Mutex::ScopedLock lock(mUpdateSceneMutex);
    mUpdateTasks.Add(new SceneUpdateTask_AddLight(light.Get()));
}

void SceneManager::SetCamera_UpdateTask(Camera *InCamera)
{
    mCamera = InCamera;
    mOnChangeEvent();
}

void SceneManager::SetCamera(const CameraRefPtr &camera)
{
    Mutex::ScopedLock lock(mUpdateSceneMutex);
    mUpdateTasks.Add(new SceneUpdateTask_SetCamera(camera.Get()));
}

LEMath::FloatVector4 SceneManager::GetUVtoViewParameter() const
{
    if (mCamera.IsValid()) {
        return mCamera->GetUVtoViewParameter();
    }
    return LEMath::FloatVector4::Zero;
}

LEMath::FloatVector4 SceneManager::GetPerspectiveProjectionParameters() const
{
    if (mCamera.IsValid()) {
        return mCamera->GetPerspectiveProjectionParameters();
    }
    return LEMath::FloatVector4::Zero;
}

void SceneManager::Update()
{
    updateSceneTasks();

    // Get new render target
    mSceneColor = LE_RenderTargetPoolManager.GetRenderTarget(mSceneColor.GetDesc(), "SceneColor");

    mCamera->Update();

    LE_DrawManager.SetViewMatrix(mCamera->GetViewMatrix());
    LE_DrawManager.SetProjectionMatrix(mCamera->GetProjectionMatrix());
    if (mEnvironmentLight.IsValid()) {
        LE_DrawManager.SetEnvironmentReflectionMap(((LightIBL*)mEnvironmentLight.Get())->GetIBLReflectionTexture());
        LE_DrawManager.SetEnvironmentIrradianceMap(((LightIBL*)mEnvironmentLight.Get())->GetIBLIrradianceTexture());
    }
    LE_DrawManager.UpdateMatrices();
}

void SceneManager::updateSceneTasks()
{
    Mutex::ScopedLock lock(mUpdateSceneMutex);
    for (uint32 i = 0; i < mUpdateTasks.count(); i++) {
        mUpdateTasks[i]->Run(this);
        delete mUpdateTasks[i];
    }
    mUpdateTasks.Clear();
}

void SceneManager::drawBackground()
{
    DrawCommand::BeginEvent("DrawBackground");
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());

    LEMath::IntSize backgroundoffset;
    switch (mBackgroundType) {
    case BackgroundImageType::None:
        DrawCommand::ClearScreen(LEMath::FloatColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));
        break;
    case BackgroundImageType::Fullscreen: 
        if (mBackgroundImage->GetSize().Width() * mSceneColor.GetDesc().Size.Height() / mBackgroundImage->GetSize().Height() < mSceneColor.GetDesc().Size.Width()) { // Landscape
            backgroundoffset = LEMath::IntSize(
                0, (mBackgroundImage->GetSize().Height() * mSceneColor.GetDesc().Size.Width() / mBackgroundImage->GetSize().Width() - mSceneColor.GetDesc().Size.Height()) / 2
            );
        }
        else { // Portrait
            backgroundoffset = LEMath::IntSize(
                (mBackgroundImage->GetSize().Width() * mSceneColor.GetDesc().Size.Height() / mBackgroundImage->GetSize().Height() - mSceneColor.GetDesc().Size.Width()) / 2, 0
            );
        }
    case BackgroundImageType::Longlat:
    {
        DrawCommand::SetViewport(LEMath::IntRect(-backgroundoffset.X(), -backgroundoffset.Y(), mSceneColor.GetDesc().Size.X() + backgroundoffset.X(), mSceneColor.GetDesc().Size.Y() + backgroundoffset.Y()));
        DrawCommand::SetScissorRect(LEMath::IntRect(0, 0, mSceneColor.GetDesc().Size.X(), mSceneColor.GetDesc().Size.Y()));
        DrawCommand::SetPipelineState(mBackgroundPipelineStates[static_cast<uint32>(mBackgroundType)].Get());
        //DrawCommand::SetConstantBuffer(0, mConstantBuffer.Get());

        //ConstantBuffer *cb = mBackgroundConstantBuffers[(uint32)mBackgroundType].Get();

        //LEMath::IntSize ScreenSize = LE_DrawManager.GetRealScreenSize();
        //LEMath::IntSize ImageSize = mBackgroundImage->GetSize();

        //LEMath::FloatPoint ImageUVOffset;
        //LEMath::IntSize AdjustImageSize;
        //if (ScreenSize.Height() * ImageSize.Width() / ScreenSize.Width() <= ImageSize.Height()) {
        //    AdjustImageSize.SetY(ScreenSize.Height() * ImageSize.Width() / ScreenSize.Width());
        //    AdjustImageSize.SetX(AdjustImageSize.Height() * ScreenSize.Width() / ScreenSize.Height());
        //    ImageUVOffset = LEMath::FloatPoint(0.0f, (float)(AdjustImageSize.Y() - ScreenSize.Y()) * 0.5f / ScreenSize.Y());
        //}
        //else {
        //    AdjustImageSize.SetX(ScreenSize.Width() * ImageSize.Height() / ScreenSize.Height());
        //    AdjustImageSize.SetY(AdjustImageSize.Width() * ScreenSize.Height() / ScreenSize.Width());
        //    ImageUVOffset = LEMath::FloatPoint((float)(ImageSize.X() - AdjustImageSize.X()) * 0.5f / ImageSize.X(), 0.0f);
        //}

        if (mBackgroundImage.IsValid()) {
            DrawCommand::BindSampler(0, SamplerState::Get(SamplerStateDesc()));
            DrawCommand::BindTexture(0, mBackgroundImage.Get());
        }
        //DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::SOURCE);
        //DrawCommand::SetDepthFunc(RendererFlag::TestFlags::ALWAYS);

        //if (mBackgroundType == BackgroundImageType::Fullscreen && shader) {
        //    if (mBackgroundColorCovertParameterIndex < 0) {
        //        mBackgroundColorCovertParameterIndex = shader->GetUniformLocation("ColorConvertParameters");
        //        cb->Create(shader);
        //    }
        //    if (mBackgroundColorCovertParameterIndex >= 0) {
        //        DrawCommand::SetShaderUniformFloat4(shader, cb, mBackgroundColorCovertParameterIndex, mBackgroundColorCovertParameter);
        //    }
        //}

        LE_Draw2DManager.DrawScreen();
    }   break;
    }
    DrawCommand::EndEvent();
}

void SceneManager::drawPrePass()
{
    DrawCommand::BeginEvent("Prepass");
    RenderState PrePassRenderState = LE_DrawManager.GetRenderState();
    PrePassRenderState.SetRenderPass(RenderPass::PrePass);
    DrawCommand::SetRenderTarget(0, mSceneNormal.Get(), mSceneDepth.Get());
    PrePassRenderState.SetRenderTarget(0, mSceneNormal.Get(), mSceneDepth.Get());
    PrePassRenderState.SetDepthEnabled(true);
    PrePassRenderState.SetDepthWriteMask(RendererFlag::DepthWriteMask::All);
    PrePassRenderState.SetDepthFunc(RendererFlag::TestFlags::LEqual);
    for (uint32 mdlidx = 0; mdlidx < mModels.count(); mdlidx++) {
        mModels[mdlidx]->Draw(PrePassRenderState);
    }
    DrawCommand::EndEvent();
}

PooledRenderTarget SceneManager::drawAmbientOcclusion()
{
    DrawCommand::BeginEvent("AmbientOcclusion");
    PostProcessContext Context;
    Context.RenderStateContext = LE_DrawManager.GetRenderStatePtr();
    Context.SceneColor = LE_SceneManager.GetSceneColor();
    Context.SceneDepth = mSceneDepth;
    Context.SceneNormal = mSceneNormal;
    Context.BlueNoiseTexture = LE_DrawManager.GetBlueNoiseTexture();
    Context.BlueNoiseContext = LE_DrawManager.GetBlueNoiseContext();
    Context.FrameIndexContext = LE_DrawManager.GetFrameIndexContext();
    VectorArray<PooledRenderTarget> AORenderTargets;
    AORenderTargets.Add();
    mAmbientOcclusion->Process(Context, AORenderTargets);
    if (AORenderTargets[0].Get()) {
        AORenderTargets[0].Get()->SetDebugName("AORenderTarget");
    }
    DrawCommand::EndEvent();
    return AORenderTargets[0];
}

void SceneManager::drawBasePass()
{
    DrawCommand::BeginEvent("Basepass");
    RenderState BasePassRenderState = LE_DrawManager.GetRenderState();
    BasePassRenderState.SetRenderPass(RenderPass::BasePass);
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
    BasePassRenderState.SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
    BasePassRenderState.SetDepthEnabled(true);
    BasePassRenderState.SetDepthWriteMask(RendererFlag::DepthWriteMask::Zero);
    BasePassRenderState.SetDepthFunc(RendererFlag::TestFlags::Equal);
    //DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::ALPHABLEND);
    for (uint32 mdlidx = 0; mdlidx < mModels.count(); mdlidx++) {
        mModels[mdlidx]->Draw(BasePassRenderState);
    }
    DrawCommand::EndEvent();
}

void SceneManager::drawTranslucencyPass()
{
    DrawCommand::BeginEvent("Translucency");
    RenderState TranslucencyRenderState = LE_DrawManager.GetRenderState();
    TranslucencyRenderState.SetRenderPass(RenderPass::TranslucencyPass);
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
    //DrawCommand::SetEnable((uint32)RendererFlag::EnabledFlags::DEPTH_WRITE);
    //DrawCommand::SetDepthFunc(RendererFlag::TestFlags::LEQUAL);
    //DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::ALPHABLEND);
    for (uint32 mdlidx = 0; mdlidx < mModels.count(); mdlidx++) {
        mModels[mdlidx]->Draw(TranslucencyRenderState);
    }
    DrawCommand::EndEvent();
}

ModelInstRefPtr SceneManager::findModelInstance(uint32 InstanceID)
{
    ModelInstRefPtr Output;
    for (uint32 ModelIndex = 0; ModelIndex < mModels.count(); ModelIndex++)
    {
        if (mModels[ModelIndex]->GetInstanceID() == InstanceID) {
            Output = mModels[ModelIndex];
            break;
        }
    }
    return Output;
}

void SceneManager::Draw()
{
    // Release pending render targets
    uint32 PendingDeleteRenderTargetSlot = 0xffff;
    for (uint32 RTIndex = 0; RTIndex < PendingDeleteRenderTargetCount; RTIndex++) {
        if (mPendingReleaseRenderTargets[RTIndex].Get() && mPendingReleaseRenderTargets[RTIndex].Get()->GetReferenceCounter() <= 1) {
            LEASSERT(mPendingReleaseRenderTargets[RTIndex].Get()->GetReferenceCounter() == 1);
            mPendingReleaseRenderTargets[RTIndex].Release();
        }
        else if (mPendingReleaseRenderTargets[RTIndex].Get() == nullptr && PendingDeleteRenderTargetSlot == 0xffff) {
            PendingDeleteRenderTargetSlot = RTIndex;
        }
    }
    LEASSERT(PendingDeleteRenderTargetSlot != 0xffff);

    DrawCommand::BeginEvent("Scene");
    DrawCommand::BeginScene();
    DrawCommand::ResourceBarrier(mSceneNormal.Get(), ResourceState::RenderTarget);
    DrawCommand::ResourceBarrier(mSceneColor.Get(), ResourceState::RenderTarget);
    DrawCommand::ResourceBarrier(mSceneDepth.Get(), ResourceState::DepthWrite);
    drawBackground();
    drawPrePass();
    //PooledRenderTarget AORenderTarget = drawAmbientOcclusion();
    //if (AORenderTarget.Get())
    //    mPendingReleaseRenderTargets[PendingDeleteRenderTargetSlot] = AORenderTarget;
    //LE_DrawManager.SetAmbientOcclusionTexture(AORenderTarget);
    drawBasePass();
    //drawTranslucencyPass();
    DrawCommand::EndScene();
    DrawCommand::ResourceBarrier(mSceneNormal.Get(), ResourceState::GenericRead);
    DrawCommand::ResourceBarrier(mSceneColor.Get(), ResourceState::GenericRead);
    DrawCommand::ResourceBarrier(mSceneDepth.Get(), ResourceState::DepthRead);
    //DrawCommand::SetRenderTarget(0, static_cast<TextureInterface*>(LE_DrawManager.GetFrameBufferTexture().Get()), nullptr);
    DrawCommand::EndEvent();
}

void SceneManager::DrawDebugUI(Font *SystemFont)
{
/*
    char InfoText[0xff];
    sprintf_s<0xff>(InfoText, "Pos : %0.02f %0.02f %0.02f", mCamera->GetPosition().X(), mCamera->GetPosition().Y(), mCamera->GetPosition().Z());
    SystemFont->Draw(LEMath::IntPoint(10, 100), InfoText);

    sprintf_s<0xff>(InfoText, "Backgound : %0.02f %0.02f %0.02f %0.02f", mBackgroundColorCovertParameter.X(), mBackgroundColorCovertParameter.Y(), mBackgroundColorCovertParameter.Z(), mBackgroundColorCovertParameter.W());
    SystemFont->Draw(LEMath::IntPoint(10, 120), InfoText);
*/
}
}