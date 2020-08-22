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
#include "Renderer/ModelInstance.h"
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
    static constexpr uint32 PendingDeleteRenderTargetCount = 0xfu;

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

	size_t          GetModelCount()				 { return mModels.size();}
    uint32          AddModel(const ModelRefPtr &m);
    void            AddLight(const LightRefPtr &l);
    CameraRefPtr    GetCamera() const               { return mCamera; }
    void            SetCamera(const CameraRefPtr &c);

    void UpdateModelTransform(uint32 InstanceID, const Transform &InTransform);

    const PooledRenderTarget& GetSceneColor() const { return mSceneColor; }
    const PooledDepthStencil& GetSceneDepth() const { return mSceneDepth; }
    const PooledRenderTarget& GetSceneNormal() const { return mSceneNormal; }

    void SetBackgroundImage(const TextureRefPtr &Background, BackgroundImageType Type);

	void AddOnChangeEventListener(const EventFunctionType &func) { mOnChangeEvent += func; }
	void RemoveOnChangeEventListener(const EventFunctionType &func) { mOnChangeEvent -= func; }

    LEMath::FloatVector4 GetUVtoViewParameter() const;
    LEMath::FloatVector4 GetPerspectiveProjectionParameters() const;

    void Init(const InitializeOptions &InitOptions);
    void PostInit(const InitializeOptions &InitOptions);
    void Update();
    void Draw();
    void DrawDebugUI(Font *SystemFont);

    void SetBackgroundExposure(float InExposure) { mBackgroundColorCovertParameter.SetW(InExposure); }
    void SetBackgroundScaleXYZ(const LEMath::FloatVector3 &InXYZScale)
    {
        mBackgroundColorCovertParameter.SetX(InXYZScale.X());
        mBackgroundColorCovertParameter.SetY(InXYZScale.Y());
        mBackgroundColorCovertParameter.SetZ(InXYZScale.Z());
    }

    void AddModel_UpdateTask(Model *InModel, uint32 InID);
    void AddLight_UpdateTask(Light *InLight);
    void SetCamera_UpdateTask(Camera *InCamera);

private:
    void updateSceneTasks();
    void drawBackground();
    void drawPrePass();
    PooledRenderTarget drawAmbientOcclusion();
    void drawBasePass();
    void drawTranslucencyPass();

    ModelInstRefPtr findModelInstance(uint32 InstanceID);

private:
    Mutex                               mUpdateSceneMutex;

    CameraRefPtr                        mCamera;
    VectorArray<ModelInstRefPtr>        mModels;
    VectorArray<LightRefPtr>            mLights;
    LightRefPtr                         mEnvironmentLight;

    uint32                              mCurrentInstanceID;

    ReferenceCountedPointer<Texture>    mBackgroundImage;
    BackgroundImageType                 mBackgroundType;
    VectorArray<ShaderRefPtr>           mBackgroundShaders;
    VectorArray<ConstantBufferRefPtr>   mBackgroundConstantBuffers;

    PooledRenderTarget                  mPendingReleaseRenderTargets[PendingDeleteRenderTargetCount];      //!< Pending release for draw thread

    PooledRenderTarget                  mSceneNormal;
    PooledRenderTarget                  mSceneColor;
    PooledDepthStencil                  mSceneDepth;

    class PostProcessAmbientOcclusion  *mAmbientOcclusion;

    int                                 mBackgroundColorCovertParameterIndex;
    LEMath::FloatVector4                mBackgroundColorCovertParameter;

    VectorArray<SceneUpdateTask*>       mUpdateTasks;

private:
	EventListener				        mOnChangeEvent;

    friend struct SceneUpdateTask;
    friend class SceneUpdateTask_UpdateModelTransform;
};
#define LE_SceneManager LimitEngine::SceneManager::GetSingleton()
}
