/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LimitEngine.h
 @brief Main Class of LimitEngine
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#pragma once

#include <LEPlatform>
#include <LEManagers>
#include <LERenderer>

#include "InitializeOptions.h"
#include "Core/Singleton.h"
#include "Core/ReferenceCountedPointer.h"
#include "Core/SerializableResource.h"
#include "Managers/TaskManager.h"
#include "Factories/ResourceFactory.h"
#include "Renderer/Transform.h"
#include "Renderer/SerializableRendererResource.h"
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
    void Init(WINDOW_HANDLE handle, const InitializeOptions &Options);
    void Term();

    void SetResourceRootPath(const char *RootPath);
    void SetBackgroundImage(const TextureRefPtr &Image, BackgroundImageType Type);
    void SetBackgroundXYZScale(const LEMath::FloatVector3 &InScale);
    void SetBackgroundExposure(float InExposure);

    void SetMainCamera(const CameraRefPtr &InCamera);

    SerializableRendererResource* Load(const char *filepath, ResourceFactory::ID ID, bool bTransient);
    Texture* LoadTexture(const char *filepath, ResourceFactory::ID ResourceID, bool bTransient);
    Model* LoadModel(const char *filepath, ResourceFactory::ID ResourceID, bool bTransient);

    uint32 AddModel(const ModelRefPtr &InModel);
    void AddLight(const LightRefPtr &InLight);

    void UpdateModelTransform(uint32 InstanceID, const Transform &InTransform);

    void Update();

    void Suspend();
    void Resume();

    void DrawDebugUI();

private: // Private members
    ResourceManager                 *mResourceManager;
    DrawManager		                *mDrawManager;
    SceneManager                    *mSceneManager;
    ShaderManager                   *mShaderManager;
    PostProcessManager              *mPostProcessManager;
    TaskManager                     *mTaskManager;
    RenderTargetPoolManager         *mRenderTargetPoolManager;
    //GUIManager                      *mGuiManager;
    //ProfileManager                  *mProfileManager;
    //PostFilterManager               *mPostFilterManager;

    TaskManager::TaskID              mTaskID_UpdateScene;                //!< Task for updating scene
    TaskManager::TaskID              mTaskID_DrawScene;                  //!< Task for drawing scene
    TaskManager::TaskID              mTaskID_PostProcess;               //!< Task for post process
    TaskManager::TaskID              mTaskID_DrawManager_Run;            //!< Task for running drawmanager
    TaskManager::TaskID              mTaskID_DrawDebugUI;               //!< Task for drawing debug UI
    TaskManager::TaskID              mTaskID_DrawEnd;

    FontRefPtr                       mSystemFont;
    TextureRefPtr                    mBlueNoiseTexture; 
    TextureRefPtr                    mEnvironmentBRDF;

	class Debug				        *mDebug;
}; // Main
}
#define gLimitEngine LimitEngine::LimitEngine::GetSingleton()
