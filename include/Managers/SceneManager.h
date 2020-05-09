/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  SceneManager.h
 @brief SceneManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include "InitializeOptions.h"

#include "Core/Singleton.h"
#include "Core/EventListener.h"
#include "Core/ReferenceCountedPointer.h"
#include "Core/Mutex.h"
#include "Containers/VectorArray.h"
#include "Renderer/Camera.h"
#include "Renderer/Model.h"
#include "Renderer/Light.h"
#include "Managers/TaskManager.h"
#include "Managers/RenderTargetPoolManager.h"

#define s_SceneManager LimitEngine::SceneManager::GetSingleton()

namespace LimitEngine {
class Camera;
class Light;
class Model;
class SceneFactory;
class SceneManager;
typedef Singleton<SceneManager, LimitEngineMemoryCategory_Graphics> SingletonSceneManager;
class SceneManager : public SingletonSceneManager
{
    friend SceneFactory;

public:
    struct SceneUpdateTask : public Object<LimitEngineMemoryCategory_Graphics>
    {
    public:
        virtual ~SceneUpdateTask() {}
        virtual void Run(class SceneManager *Manager) = 0;
    };

public:
    SceneManager();
    ~SceneManager();

	size_t       GetModelCount()				 { return mModels.size();}
	ModelRefPtr  GetModel(uint32 n)				 { return mModels[n];}
    void         AddModel(const ModelRefPtr &m);
    void         AddLight(const LightRefPtr &l);
    CameraRefPtr GetCamera() const               { return mCamera; }
    void         SetCamera(const CameraRefPtr &c);

    const PooledRenderTarget& GetSceneColor() const { return mSceneColor; }
    const PooledDepthStencil& GetSceneDepth() const { return mSceneDepth; }

    void    SetBackgroundImage(const TextureRefPtr &Background, BackgroundImageType Type);

	void	AddOnChangeEventListener(const EventFunctionType &func) { mOnChangeEvent += func; }
	void	RemoveOnChangeEventListener(const EventFunctionType &func) { mOnChangeEvent -= func; }

    void Init(const InitializeOptions &InitOptions);
    void Update();
    void Draw();

    void AddModel_UpdateTask(Model *InModel);
    void AddLight_UpdateTask(Light *InLight);
    void SetCamera_UpdateTask(Camera *InCamera);

private:
    void LoadFromText(const char *text);

    void updateSceneTasks();
    void drawBackground();

private:
    Mutex                            mUpdateSceneMutex;

    CameraRefPtr                     mCamera;
    VectorArray<ModelRefPtr>         mModels;
    VectorArray<LightRefPtr>         mLights;
    LightRefPtr                      mEnvironmentLight;

    ReferenceCountedPointer<Texture> mBackgroundImage;
    BackgroundImageType              mBackgroundType;
    VectorArray<ShaderRefPtr>        mBackgroundShaders;

    PooledRenderTarget               mSceneColor;
    PooledDepthStencil               mSceneDepth;

    VectorArray<SceneUpdateTask*>    mUpdateTasks;

private:
	EventListener				     mOnChangeEvent;

    friend SceneUpdateTask;
};
#define LE_SceneManager LimitEngine::SceneManager::GetSingleton()
}
