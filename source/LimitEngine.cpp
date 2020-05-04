/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LimitEngine.cpp
 @brief Main Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "LimitEngine.h"

#include "Core/AutoPointer.h"
#include "Core/Common.h"
#include "Core/Debug.h"
#include "Core/TaskPriority.h"
#include "Factories/ArchiveFactory.h"
#include "Managers/DrawManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderDriverManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/PostProcessManager.h"
#include "Managers/TaskManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/RenderTargetPoolManager.h"
#include "Renderer/Font.h"
//#include "Random.h"
//#include "TouchInterface.h"
//#include "Timer.h"

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
    mSystemFont->InitResource();

	mTaskID_UpdateScene     = LE_TaskManager.AddTask("SceneManager::Update",    TaskPriority_Renderer_UpdateScene,      mSceneManager, &SceneManager::Update);
	mTaskID_DrawScene       = LE_TaskManager.AddTask("SceneManager::Draw",      TaskPriority_Renderer_DrawScene,        mSceneManager, &SceneManager::Draw);
    mTaskID_PostProcess     = LE_TaskManager.AddTask("PostProcess::Process",    TaskPriority_Renderer_PostProcess,      mPostProcessManager, &PostProcessManager::Process);
    mTaskID_DrawDebugUI     = LE_TaskManager.AddTask("LimitEngine::DrawDebugUI",TaskPriority_Renderer_DrawDebugUI, this, &LimitEngine::DrawDebugUI);
    mTaskID_DrawManager_Run = LE_TaskManager.AddTask("DrawManager::Run",        TaskPriority_Renderer_DrawManager_Run,  mDrawManager,  &DrawManager::Run);
}

void LimitEngine::Term()
{
	if (mTaskID_UpdateScene)
		LE_TaskManager.RemoveTask(mTaskID_UpdateScene);
//	if (mTaskID_UpdateLight)
//		LE_TaskManager.RemoveTask(mTaskID_UpdateLight);
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
    if (OutModel) {
        mSceneManager->AddModel(OutModel);
    }
    return OutModel;
}
void LimitEngine::AddModel(const ModelRefPtr &InModel)
{
    if (mSceneManager) {
        mSceneManager->AddModel(InModel);
    }
}
void LimitEngine::AddLight(const LightRefPtr &InLight)
{
    if (mSceneManager) {
        mSceneManager->AddLight(InLight);
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
, mShaderDriverManager(nullptr)
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
	mShaderDriverManager = new ShaderDriverManager();
	//mGuiManager = new GUIManager();
	//mProfileManager = new ProfileManager();
 //   mLightManager = new LightManager();

 //   //! random initialize
 //   Random::init(static_cast<uint32>(Timer::GetTimeUSec()));
}

LimitEngine::~LimitEngine()
{
    //mResourceManager->SaveResource("fonts/System.font.lea", mSystemFont);

	//delete mResourceManager; mResourceManager = NULL;
	delete mDrawManager; mDrawManager = nullptr;
    delete mPostProcessManager; mPostProcessManager = nullptr;
	delete mShaderManager; mShaderManager = nullptr;
    delete mSceneManager; mSceneManager = nullptr;
    delete mShaderDriverManager; mShaderDriverManager = NULL;
	//delete mGuiManager; mGuiManager = NULL;
	//delete mProfileManager; mProfileManager = NULL;
	//delete mLightManager; mLightManager = NULL;
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
    mSystemFont->Draw(LEMath::IntPoint(10, 10), "LimitEngine");
}
}