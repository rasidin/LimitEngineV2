/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file LimitEngine.cpp
@brief Main of LimitEngine
@author minseob (leeminseob@outlook.com)
**********************************************************************/

#include "LimitEngine.h"

#include "Core/AutoPointer.h"
#include "Core/Common.h"
#include "Core/Debug.h"
#include "Core/TaskPriority.h"
#include "Factories/ArchiveFactory.h"
#include "Factories/TextureFactory.h"
#include "Managers/DrawManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/PostProcessManager.h"
#include "Managers/TaskManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/RenderTargetPoolManager.h"
#include "Renderer/Font.h"
#include "Renderer/SamplerState.h"
#include "Renderer/PipelineState.h"

namespace LimitEngine {
// =============================================================
// Main
LimitEngine* LimitEngine::mInstance = NULL;
void LimitEngine::Init(WINDOW_HANDLE handle, const InitializeOptions &Options)
{
	mSceneManager->Init(Options);
	mDrawManager->Init(handle, Options);
	mShaderManager->Init();
    mPostProcessManager->Init(Options);

	mTaskManager->Init();

    mSystemFont = Font::GenerateFromFile("fonts/System.tga", "fonts/System.text");
    //if (const ResourceManager::RESOURCE *LoadedResource = LE_ResourceManager.GetResourceWithRegister("fonts/System.font.lea", ArchiveFactory::ID)) {
    //    mSystemFont = (Font*)(LoadedResource->data);
    //}
    //mSystemFont->InitResource();

    //if (TextureFactory *Factory = (TextureFactory*)ResourceManager::GetSingleton().GetFactory(TextureFactory::ID)) {
    //    Factory->SetImportFilter(TextureFactory::TextureImportFilter::EnvironmentBRDF);
    //    Factory->SetSizeFilteredImage(LEMath::IntVector2(128, 128));
    //    Factory->SetSampleCount(256);
    //}
    //if (const ResourceManager::RESOURCE *LoadedResource = LE_ResourceManager.GetResourceWithRegister("textures/EnvironmentBRDF.texture.lea", ArchiveFactory::ID)) {
    //    mEnvironmentBRDF = dynamic_cast<Texture*>(LoadedResource->data);
    //    mEnvironmentBRDF->InitResource();
    //    mDrawManager->SetEnvironmentBRDFTexture(mEnvironmentBRDF);
    //}

    //if (const ResourceManager::RESOURCE *LoadedResource = LE_ResourceManager.GetResourceWithRegister("textures/BlueNoise.texture.text", TextureFactory::ID)) {
    //    mBlueNoiseTexture = dynamic_cast<Texture*>(LoadedResource->data);
    //    mBlueNoiseTexture->InitResource();
    //    mDrawManager->SetBlueNoiseTexture(mBlueNoiseTexture);
    //    mDrawManager->SetBlueNoiseContext(LEMath::FloatVector4(
    //        1.0f / mBlueNoiseTexture.Get()->GetSize().Width(),
    //        1.0f / mBlueNoiseTexture.Get()->GetSize().Height(), 
    //        1.0f / 8.0f, 0.0f));
    //}

    mSceneManager->PostInit(Options);

	mTaskID_UpdateScene     = LE_TaskManager.AddTask("SceneManager::Update",    TaskPriority_Renderer_UpdateScene,      mSceneManager,          &SceneManager::Update);
	mTaskID_DrawScene       = LE_TaskManager.AddTask("SceneManager::Draw",      TaskPriority_Renderer_DrawScene,        mSceneManager,          &SceneManager::Draw);
    mTaskID_PostProcess     = LE_TaskManager.AddTask("PostProcess::Process",    TaskPriority_Renderer_PostProcess,      mPostProcessManager,    &PostProcessManager::Process);
    mTaskID_DrawDebugUI     = LE_TaskManager.AddTask("LimitEngine::DrawDebugUI",TaskPriority_Renderer_DrawDebugUI,      this,                   &LimitEngine::DrawDebugUI);
    mTaskID_DrawEnd         = LE_TaskManager.AddTask("DrawEnd",                 TaskPriority_Renderer_DrawEnd,          mDrawManager,           &DrawManager::DrawEnd);
    mTaskID_DrawManager_Run = LE_TaskManager.AddTask("DrawManager::Run",        TaskPriority_Renderer_DrawManager_Run,  mDrawManager,           &DrawManager::Run);
}

void LimitEngine::Term()
{
	if (mTaskID_UpdateScene)
		LE_TaskManager.RemoveTask(mTaskID_UpdateScene);
	if (mTaskID_DrawScene)
		LE_TaskManager.RemoveTask(mTaskID_DrawScene);
	if (mTaskID_DrawManager_Run)
		LE_TaskManager.RemoveTask(mTaskID_DrawManager_Run);
    if (mTaskID_PostProcess)
        LE_TaskManager.RemoveTask(mTaskID_PostProcess);
    if (mTaskID_DrawDebugUI)
        LE_TaskManager.RemoveTask(mTaskID_DrawDebugUI);

	mTaskManager->Term();
	mShaderManager->Term();

    SamplerState::TerminateCache();
}
void LimitEngine::SetResourceRootPath(const char *RootPath)
{
    if (mResourceManager) {
        mResourceManager->SetRootPath(RootPath);
    }
}
void LimitEngine::SetBackgroundImage(const TextureRefPtr &Image, BackgroundImageType Type)
{
    if (mSceneManager) {
        mSceneManager->SetBackgroundImage(Image, Type);
    }
}
void LimitEngine::SetBackgroundXYZScale(const LEMath::FloatVector3 &InScale)
{
    if (mSceneManager) {
        mSceneManager->SetBackgroundScaleXYZ(InScale);
    }
}
void LimitEngine::SetBackgroundExposure(float InExposure)
{
    if (mSceneManager) {
        mSceneManager->SetBackgroundExposure(InExposure);
    }
}
void LimitEngine::SetMainCamera(const CameraRefPtr &InCamera)
{
    mSceneManager->SetCamera(InCamera);
}
Texture* LimitEngine::LoadTexture(const char *filepath, ResourceFactory::ID ID, bool bTransient)
{
    if (bTransient) {
        if (AutoPointer<ResourceManager::RESOURCE> TextureResource = mResourceManager->GetResourceWithoutRegister(filepath, ID)) {
            return (Texture*)TextureResource->PopData();
        }
    }
    else {
        if (const ResourceManager::RESOURCE *TextureResource = mResourceManager->GetResourceWithRegister(filepath, ID)) {
            return (Texture*)TextureResource->data;
        }
    }
    return nullptr;
}
Model* LimitEngine::LoadModel(const char *filepath, ResourceFactory::ID ID, bool bTransient)
{
    Model* OutModel = nullptr;
    if (bTransient) {
        if (AutoPointer<ResourceManager::RESOURCE> TextureResource = mResourceManager->GetResourceWithoutRegister(filepath, ID)) {
            OutModel = (Model*)TextureResource->PopData();
        }
    }
    else {
        if (const ResourceManager::RESOURCE *TextureResource = mResourceManager->GetResourceWithRegister(filepath, ID)) {
            OutModel = (Model*)TextureResource->data;
        }
    }
    return OutModel;
}
uint32 LimitEngine::AddModel(const ModelRefPtr &InModel)
{
    if (mSceneManager) {
        return mSceneManager->AddModel(InModel);
    }
    return InstanceIDNone;
}
void LimitEngine::AddLight(const LightRefPtr &InLight)
{
    if (mSceneManager) {
        mSceneManager->AddLight(InLight);
    }
}
void LimitEngine::UpdateModelTransform(uint32 InstanceID, const Transform &InTransform)
{
    if (mSceneManager) {
        mSceneManager->UpdateModelTransform(InstanceID, InTransform);
    }
}
void LimitEngine::Suspend()
{
}

void LimitEngine::Resume()
{
}

LimitEngine::LimitEngine()
: Singleton<LimitEngine>()
, mDrawManager(nullptr)
, mShaderManager(nullptr)
, mSceneManager(nullptr)
, mPostProcessManager(nullptr)
, mTaskManager(nullptr)
, mRenderTargetPoolManager(nullptr)
, mResourceManager(NULL)
//, mLightManager(NULL)
//, mPostFilterManager(NULL)
//, mDebug(NULL)
, mTaskID_UpdateScene(nullptr)
, mTaskID_DrawScene(nullptr)
, mTaskID_DrawManager_Run(nullptr)
, mSystemFont(nullptr)
, mDebug(nullptr)
{
	mDebug = new Debug();

	gDebug << "Ready LimitEngine...." << Debug::EndL;
	gDebug << "Date : " << __DATE__ << Debug::EndL;
    
	mResourceManager = new ResourceManager();
    mRenderTargetPoolManager = new RenderTargetPoolManager();
    mTaskManager = new TaskManager();
	mSceneManager = new SceneManager();
	mDrawManager = new DrawManager();
	mShaderManager = new ShaderManager();
    mPostProcessManager = new PostProcessManager();
	//mGuiManager = new GUIManager();
	//mProfileManager = new ProfileManager();
 //   mLightManager = new LightManager();

 //   //! random initialize
 //   Random::init(static_cast<uint32>(Timer::GetTimeUSec()));
}

LimitEngine::~LimitEngine()
{
	delete mResourceManager; mResourceManager = NULL;
	delete mDrawManager; mDrawManager = nullptr;
    delete mPostProcessManager; mPostProcessManager = nullptr;
	delete mShaderManager; mShaderManager = nullptr;
    delete mSceneManager; mSceneManager = nullptr;
    delete mRenderTargetPoolManager; mRenderTargetPoolManager = nullptr;

	delete mTaskManager; mTaskManager = nullptr;
	delete mDebug; mDebug = nullptr;
}

void LimitEngine::Update()
{
    //if (mTaskManager) {
    //    mTaskManager->Run();
    //}
}

void LimitEngine::DrawDebugUI()
{
    //mSystemFont->Draw(LEMath::IntPoint(10, 10), "LimitEngine");
    //if (mSceneManager) {
    //    mSceneManager->DrawDebugUI(mSystemFont.Get());
    //}
}
}