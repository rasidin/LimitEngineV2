/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  SceneManager.cpp
 @brief SceneManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
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
#include "Renderer/Model.h"
#include "Renderer/ModelInstance.h"
#include "Renderer/Light.h"
#include "Renderer/LightIBL.h"
#include "Renderer/Texture.h"

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
    , mAmbientOcclusion(nullptr)
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
    mBackgroundShaders.Resize(static_cast<uint32>(BackgroundImageType::Num));
    mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Fullscreen)] = LE_ShaderManager.GetShader("DrawFullscreen");
    mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Longlat)] = LE_ShaderManager.GetShader("Draw2D");

    TEXTURE_COLOR_FORMAT SceneColorFormat = (InitOptions.SceneColorSpace == InitializeOptions::ColorSpace::Linear) ? TEXTURE_COLOR_FORMAT_A16B16G16R16F : TEXTURE_COLOR_FORMAT_A8R8G8B8;
    TEXTURE_COLOR_FORMAT SceneNormalFormat = TEXTURE_COLOR_FORMAT_A16B16G16R16F;
    mSceneColor = LE_RenderTargetPoolManager.GetRenderTarget(InitOptions.Resolution, 1, SceneColorFormat);
    mSceneNormal = LE_RenderTargetPoolManager.GetRenderTarget(InitOptions.Resolution, 1, SceneNormalFormat);
    mSceneDepth = LE_RenderTargetPoolManager.GetDepthStencil(InitOptions.Resolution, TEXTURE_DEPTH_FORMAT_D32F);
}

void SceneManager::PostInit(const InitializeOptions &InitOptions)
{
    PostProcessAmbientOcclusion *CapturedAmbientOcclusion = mAmbientOcclusion;
    LE_DrawManager.AddRendererTaskLambda([CapturedAmbientOcclusion, InitOptions]() {
        CapturedAmbientOcclusion->Init(InitOptions);
    });
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
    mSceneColor = LE_RenderTargetPoolManager.GetRenderTarget(mSceneColor.GetDesc());

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
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
    DrawCommand::SetEnable((uint32)RendererFlag::EnabledFlags::DEPTH_WRITE);
    DrawCommand::SetDepthFunc(RendererFlag::TestFlags::ALWAYS);

    switch (mBackgroundType) {
    case BackgroundImageType::None:
        DrawCommand::ClearScreen(LEMath::FloatColorRGBA(0.0f, 0.0f, 0.0f, 1.0f));
        break;
    case BackgroundImageType::Fullscreen: 
    case BackgroundImageType::Longlat:
    {
        LEMath::IntSize ScreenSize = LE_DrawManager.GetRealScreenSize();
        LEMath::IntSize ImageSize = mBackgroundImage->GetSize();

        LEMath::FloatPoint ImageUVOffset;
        LEMath::IntSize AdjustImageSize;
        if (ScreenSize.Height() * ImageSize.Width() / ScreenSize.Width() <= ImageSize.Height()) {
            AdjustImageSize.SetY(ScreenSize.Height() * ImageSize.Width() / ScreenSize.Width());
            AdjustImageSize.SetX(AdjustImageSize.Height() * ScreenSize.Width() / ScreenSize.Height());
            ImageUVOffset = LEMath::FloatPoint(0.0f, (float)(AdjustImageSize.Y() - ScreenSize.Y()) * 0.5f / ScreenSize.Y());
        }
        else {
            AdjustImageSize.SetX(ScreenSize.Width() * ImageSize.Height() / ScreenSize.Height());
            AdjustImageSize.SetY(AdjustImageSize.Width() * ScreenSize.Height() / ScreenSize.Width());
            ImageUVOffset = LEMath::FloatPoint((float)(ImageSize.X() - AdjustImageSize.X()) * 0.5f / ImageSize.X(), 0.0f);
        }

        if (mBackgroundImage.IsValid()) DrawCommand::BindTexture(0, mBackgroundImage.Get());
        DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::SOURCE);
        DrawCommand::SetDepthFunc(RendererFlag::TestFlags::ALWAYS);

        Vertex2D *buffer = LE_Draw2DManager.GetVertexBuffer2D(6);
        buffer[0].SetPosition(LEMath::FloatVector3(-ImageUVOffset.X(), -ImageUVOffset.Y(), 1.0f));
        buffer[0].SetColor(0xffffffff);
        buffer[0].SetTexcoord(LEMath::FloatVector2(0.0f, 0.0f));
        buffer[1].SetPosition(LEMath::FloatVector3(-ImageUVOffset.X(), 1.0f + ImageUVOffset.Y(), 1.0f));
        buffer[1].SetColor(0xffffffff);
        buffer[1].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
        buffer[2].SetPosition(LEMath::FloatVector3(1.0f + ImageUVOffset.X(), -ImageUVOffset.Y(), 1.0f));
        buffer[2].SetColor(0xffffffff);
        buffer[2].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
        buffer[3].SetPosition(LEMath::FloatVector3(-ImageUVOffset.X(), 1.0f + ImageUVOffset.Y(), 1.0f));
        buffer[3].SetColor(0xffffffff);
        buffer[3].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
        buffer[4].SetPosition(LEMath::FloatVector3(1.0f + ImageUVOffset.X(), 1.0f + ImageUVOffset.Y(), 1.0f));
        buffer[4].SetColor(0xffffffff);
        buffer[4].SetTexcoord(LEMath::FloatVector2(1.0f, 1.0f));
        buffer[5].SetPosition(LEMath::FloatVector3(1.0f + ImageUVOffset.X(), -ImageUVOffset.Y(), 1.0f));
        buffer[5].SetColor(0xffffffff);
        buffer[5].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
        LE_Draw2DManager.FlushDraw2D(RendererFlag::PrimitiveTypes::TRIANGLELIST);
    }   break;
    }
}

void SceneManager::drawPrePass()
{
    RenderState PrePassRenderState = LE_DrawManager.GetRenderState();
    PrePassRenderState.SetRenderPass(RenderPass::PrePass);
    DrawCommand::SetRenderTarget(0, mSceneNormal.Get(), mSceneDepth.Get());
    DrawCommand::SetEnable((uint32)RendererFlag::EnabledFlags::DEPTH_WRITE);
    DrawCommand::SetDepthFunc(RendererFlag::TestFlags::LEQUAL);
    for (uint32 mdlidx = 0; mdlidx < mModels.count(); mdlidx++) {
        mModels[mdlidx]->Draw(PrePassRenderState);
    }
}

PooledRenderTarget SceneManager::drawAmbientOcclusion()
{
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
    return AORenderTargets[0];
}

void SceneManager::drawBasePass()
{
    RenderState BasePassRenderState = LE_DrawManager.GetRenderState();
    BasePassRenderState.SetRenderPass(RenderPass::BasePass);
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
    DrawCommand::SetDisable((uint32)RendererFlag::EnabledFlags::DEPTH_WRITE);
    DrawCommand::SetDepthFunc(RendererFlag::TestFlags::EQUAL);
    for (uint32 mdlidx = 0; mdlidx < mModels.count(); mdlidx++) {
        mModels[mdlidx]->Draw(BasePassRenderState);
    }
}

void SceneManager::drawTranslucencyPass()
{
    RenderState TranslucencyRenderState = LE_DrawManager.GetRenderState();
    TranslucencyRenderState.SetRenderPass(RenderPass::TranslucencyPass);
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
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
            mPendingReleaseRenderTargets[RTIndex].Release();
        }
        else if (mPendingReleaseRenderTargets[RTIndex].Get() == nullptr && PendingDeleteRenderTargetSlot == 0xffff) {
            PendingDeleteRenderTargetSlot = RTIndex;
        }
    }
    LEASSERT(PendingDeleteRenderTargetSlot != 0xffff);

    DrawCommand::BeginScene();
    drawBackground();
    drawPrePass();
    PooledRenderTarget AORenderTarget = drawAmbientOcclusion();
    mPendingReleaseRenderTargets[PendingDeleteRenderTargetSlot] = AORenderTarget;
    LE_DrawManager.SetAmbientOcclusionTexture(AORenderTarget);
    drawBasePass();
    //drawTranslucencyPass();
    DrawCommand::EndScene();
    DrawCommand::SetRenderTarget(0, nullptr, nullptr);
}
}