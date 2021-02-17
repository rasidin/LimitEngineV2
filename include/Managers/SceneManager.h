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
 @file  SceneManager.h
 @brief SceneManager Class
 @author minseob (https://github.com/rasidin)
 *********************************************************************/
#pragma once

#include <LERenderer>

#include "InitializeOptions.h"

#include "Core/Singleton.h"
#include "Core/EventListener.h"
#include "Core/ReferenceCountedPointer.h"
#include "Core/Mutex.h"
#include "Containers/VectorArray.h"
#include "Renderer/Camera.h"
#include "Renderer/ConstantBuffer.h"
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
typedef Singleton<SceneManager, LimitEngineMemoryCategory::Graphics> SingletonSceneManager;
class SceneManager : public SingletonSceneManager
{
    friend SceneFactory;
    static constexpr uint32 PendingDeleteRenderTargetCount = 0xfu;

public:
    struct SceneUpdateTask : public Object<LimitEngineMemoryCategory::Graphics>
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
    VectorArray<PipelineStateRefPtr>    mBackgroundPipelineStates;
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
