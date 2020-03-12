/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LimitEngine.cpp
 @brief Main Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "LimitEngine.h"

#include "Core/Common.h"
#include "Core/Debug.h"
#include "Core/TaskPriority.h"
#include "Managers/DrawManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/TaskManager.h"
 //#include "LightManager.h"
//#include "Random.h"
//#include "SceneManager.h"
//#include "ShaderManager.h"
//#include "ShaderDriverManager.h"
//#include "TouchInterface.h"
//#include "Timer.h"

namespace LimitEngine {
// =============================================================
// Main
LimitEngine* LimitEngine::mInstance = NULL;
void LimitEngine::Init(WINDOW_HANDLE handle)
{
	mSceneManager->Init();
	mDrawManager->Init(handle);
	mShaderManager->Init();

	mTaskManager->Init();

	mTaskID_UpdateScene     = LE_TaskManager.AddTask("SceneManager::Update", TaskPriority_Renderer_UpdateScene,      mSceneManager, &SceneManager::Update);
//	mTaskID_UpdateLight     = LE_TaskManager.AddTask("LightManager::Update", TaskPriority_Renderer_UpdateLight,      mLightManager, &LightManager::Update);
	mTaskID_DrawScene       = LE_TaskManager.AddTask("SceneManager::Draw",   TaskPriority_Renderer_DrawScene,        mSceneManager, &SceneManager::Draw);
	mTaskID_DrawManager_Run = LE_TaskManager.AddTask("DrawManager::Run",     TaskPriority_Renderer_DrawManager_Run,  mDrawManager,  &DrawManager::Run);
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

	mTaskManager->Term();
	mShaderManager->Term();
}
void LimitEngine::Suspend()
{
}

void LimitEngine::Resume()
{
}

LimitEngine::LimitEngine()
: Singleton<LimitEngine>()
//: mResourceManager(NULL)
, mDrawManager(nullptr)
, mShaderManager(nullptr)
, mSceneManager(nullptr)
, mTaskManager(nullptr)
//, mLightManager(NULL)
//, mPostFilterManager(NULL)
//, mDebug(NULL)
, mTaskID_UpdateLight(nullptr)
, mTaskID_UpdateScene(nullptr)
, mTaskID_DrawScene(nullptr)
, mTaskID_DrawManager_Run(nullptr)
{
	mDebug = new Debug();

	gDebug << "Ready LimitEngine...." << Debug::EndL;
	gDebug << "Date : " << __DATE__ << Debug::EndL;
    
	//mResourceManager = new ResourceManager();
	mTaskManager = new TaskManager();
	mSceneManager = new SceneManager();
	mDrawManager = new DrawManager();
	mShaderManager = new ShaderManager();
	//mShaderDriverManager = new ShaderDriverManager();
	//mGuiManager = new GUIManager();
	//mProfileManager = new ProfileManager();
 //   mLightManager = new LightManager();
 //   mPostFilterManager = new PostFilterManager();

 //   //! random initialize
 //   Random::init(static_cast<uint32>(Timer::GetTimeUSec()));
}

LimitEngine::~LimitEngine()
{
	//delete mResourceManager; mResourceManager = NULL;
	delete mDrawManager; mDrawManager = nullptr;
	delete mShaderManager; mShaderManager = nullptr;
    delete mSceneManager; mSceneManager = nullptr;
    //delete mShaderDriverManager; mShaderDriverManager = NULL;
	//delete mGuiManager; mGuiManager = NULL;
	//delete mProfileManager; mProfileManager = NULL;
	//delete mLightManager; mLightManager = NULL;
	//delete mPostFilterManager; mPostFilterManager = NULL;
	
	delete mTaskManager; mTaskManager = nullptr;
	delete mDebug; mDebug = nullptr;
}

void LimitEngine::Update()
{
    if (mTaskManager) {
        mTaskManager->Run();
    }
}
}