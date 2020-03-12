/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LimitEngine.h
 @brief Main Class of LimitEngine
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#pragma once

#include <LEPlatform>
#include <LEManagers>
#include <LERenderer>

#include "Core/Singleton.h"
#include "Managers/TaskManager.h"
//#include "ResourceManager.h"
//#include "GuiManager.h"
//#include "ProfileManager.h"
//#include "VectorArray.h"

namespace LimitEngine {
class LimitEngine : public Singleton<LimitEngine>
{
public: // Ctor & Dtor
    explicit LimitEngine();
    virtual ~LimitEngine();

public: // Public functions
    void Init(WINDOW_HANDLE handle);
    void Term();
    
    void Update();

    void Suspend();
    void Resume();

private: // Private members
    //ResourceManager                 *mResourceManager;
    DrawManager		                *mDrawManager;
    SceneManager                    *mSceneManager;
    ShaderManager                   *mShaderManager;
    TaskManager                     *mTaskManager;
    //ShaderDriverManager             *mShaderDriverManager;
    //LightManager                    *mLightManager;
    //GUIManager                      *mGuiManager;
    //ProfileManager                  *mProfileManager;
    //PostFilterManager               *mPostFilterManager;

    TaskManager::TaskID              mTaskID_UpdateLight;                //!< Task for updating lights
    TaskManager::TaskID              mTaskID_UpdateScene;                //!< Task for updating scene
    TaskManager::TaskID              mTaskID_DrawScene;                  //!< Task for drawing scene
    TaskManager::TaskID              mTaskID_DrawManager_Run;            //!< Task for running drawmanager

	class Debug				*mDebug;
}; // Main
}
#define gLimitEngine LimitEngine::LimitEngine::GetSingleton()
