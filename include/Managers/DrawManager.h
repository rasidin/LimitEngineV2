/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  DrawManager.h
 @brief DrawManager Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#pragma once

#include <LEPlatform>
#include <LEManagers>
#include <LERenderer>

#include <LEFloatMatrix4x4.h>
#include <LEFloatVector4.h>
#include <LEIntVector2.h>

#include "InitializeOptions.h"

#include "Core/AutoPointer.h"
#include "Core/Event.h"
#include "Core/Function.h"
#include "Core/Mutex.h"
#include "Core/Singleton.h"
#include "Core/Thread.h"
#include "Core/Util.h"
#include "Containers/VectorArray.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/RenderState.h"
//#include "LE_VertexBuffer.h"
//
//#include "LE_PostFilterManager.h"

namespace LimitEngine
{
class Shader;
class Texture;
class FrameBuffer;
class DrawManagerImpl : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    DrawManagerImpl() {}
    virtual ~DrawManagerImpl() {}
    
    virtual void* MakeInitParameter() = 0;

    virtual void ReadyToRender() = 0;
    virtual LEMath::IntSize GetScreenSize() = 0;
    virtual void PreRenderFinished() = 0;
    
    virtual void Init(WINDOW_HANDLE handle, const InitializeOptions &Options) = 0;
    virtual void ResizeScreen(const LEMath::IntSize &size) = 0;
    virtual void ProcessBeforeFlushingCommands() = 0;
    virtual void Finalize(CommandBuffer* commandBuffer) = 0;
    virtual void Present() = 0;
    virtual void Term() = 0;
    
    virtual void* AllocateGPUBuffer(size_t size) = 0;
    virtual void* GetImmediateCommandList() = 0;
    virtual uint64 ExecuteImmediateCommandList(void* cmdlist, bool waitcompletion) = 0;

    virtual void* GetDeviceHandle() const = 0;
    virtual void* GetDeviceContext() const = 0;
    virtual uint32 GetCurrentFrameBufferIndex() const = 0;
    virtual const RendererFlag::BufferFormat& GetCurrentFrameBufferFormat() const = 0;
    virtual void* GetCurrentFrameBuffer() const = 0;
    virtual void* GetCurrentFrameBufferView() const = 0;
    virtual const ResourceState GetCurrentFrameBufferResourceState() const = 0;
    virtual void SetCurrentFrameBufferResourceState(ResourceState state) = 0;

    // Platform depdendency (D3D12)
    virtual void* AllocateDescriptor(uint32 type) = 0;
};

class RendererTask : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    virtual ~RendererTask() {}
    virtual void Run() = 0;

    void Release() { delete this; }
};

template<typename LAMBDA>
class RendererTaskLambda final : public RendererTask
{
public:
    RendererTaskLambda(LAMBDA &&Lambda) : mLambda(Forward<LAMBDA>(Lambda)) {}

    virtual void Run() override final {
        mLambda();
        mLambda.~LAMBDA();
    }
private:
    LAMBDA mLambda;
};

class DrawManager;
typedef Singleton<DrawManager, LimitEngineMemoryCategory::Graphics> SingletonDrawManager;
class DrawManager : public SingletonDrawManager
{
    friend DrawCommand;
    friend class DrawManagerRendererAccessor;
public:             // Public Definition
    // Screen orientation type
    enum ScreenOrientation
    {
        ScreenOrientationPortraitUpDown = 0,
        ScreenOrientationPortraitDownUp,
        ScreenOrientationLandscapeLeftRight,
        ScreenOrientationLandscapeRightLeft,
    };
    
    // Vertex type for drawing 2D
    typedef struct _VERTEX_DRAW2D
    {
        float x, y, z;            // Pos
        float u, v;                // Texcoord
        uint32 color;            // Color (RGBA32)
    } VERTEX_DRAW2D;
    
public:                    // Interfaces
    void Init(WINDOW_HANDLE handle, const InitializeOptions &Options);
    
    // Interface for task
    void Run();
    void DrawEnd();

    // Get frame buffer texture
    FrameBufferTextureRefPtr GetFrameBufferTexture() const { return new FrameBufferTexture(GetRealScreenSize()); }

    // Is ready to rendering?
    bool IsReadyToRender() { return mReadyToRender; }

    // Add renderer task (auto release)
    void AddRendererTask(AutoPointer<RendererTask> &task) { Mutex::ScopedLock lock(mRendererTaskMutex); if (RendererTask *TaskPtr = task.Pop()) mRendererTasks.Add(TaskPtr); }

    template<typename L>
    void AddRendererTaskLambda(L &&Lambda) { Mutex::ScopedLock lock(mRendererTaskMutex); mRendererTasks.Add(new RendererTaskLambda<L>(Forward<L>(Lambda))); }

    void PreRenderFinished()
    { /*if (mInstance && mInstance->mImpl) mInstance->mImpl->preRenderFinished();*/ }

    // Flush all commands (Multithread)
    void FlushCommandThread();
    // Flush all commands
    void FlushCommand();
    // Wait for flushing commands
    void WaitForFlushing();

    // Screen size
    void SetScreenOrientation(uint32 o);
    const LEMath::IntSize& GetVirtualScreenSize() { return mScreenSize; }
    LEMath::IntSize GetRealScreenSize() const { if (mImpl) return mImpl->GetScreenSize(); return LEMath::IntSize(0, 0); }

    // Set/Get matrices
    void                          SetViewMatrix(const LEMath::FloatMatrix4x4 &view)                     { mRenderState->SetViewMatrix(view); }
    const LEMath::FloatMatrix4x4& GetViewMatrix()                                                       { return mRenderState->GetViewMatrix(); }
    void                          SetProjectionMatrix(const LEMath::FloatMatrix4x4 &proj)               { mRenderState->SetProjMatrix(proj); }
    const LEMath::FloatMatrix4x4& GetProjectionMatrix()                                                 { return mRenderState->GetProjMatrix(); }
    void                          SetViewProjMatrix(const LEMath::FloatMatrix4x4 &viewProj)             { mRenderState->SetViewProjMatrix(viewProj); }
    void                          SetInvViewProjMatrix(const LEMath::FloatMatrix4x4 &invViewProj)       { mRenderState->SetInvViewProjMatrix(invViewProj); }
    void                          SetEnvironmentReflectionMap(const TextureRefPtr &InTexture)           { mRenderState->SetIBLReflectionTexture(InTexture); }
    void                          SetEnvironmentIrradianceMap(const TextureRefPtr &InTexture)           { mRenderState->SetIBLIrradianceTexture(InTexture); }
    void                          UpdateMatrices();                                                     
    
    const LEMath::IntVector4&     GetFrameIndexContext() const                                          { return mRenderState->GetFrameIndexContext(); }
    void                          SetEnvironmentBRDFTexture(const TextureRefPtr &InTexture)             { mRenderState->SetEnvironmentBRDFTexture(InTexture); }
    void                          SetAmbientOcclusionTexture(const PooledRenderTarget &InRenderTarget)  { mRenderState->SetAmbientOcclusionTexture(InRenderTarget.Get()); }
    void                          SetBlueNoiseTexture(const TextureRefPtr &InTexture)                   { mRenderState->SetBlueNoiseTexture(InTexture); }
    Texture*                      GetBlueNoiseTexture() const                                           { return mRenderState->GetBlueNoiseTexture().Get(); }
    void                          SetBlueNoiseContext(const LEMath::FloatVector4 &Context)              { mRenderState->SetBlueNoiseContext(Context); }
    const LEMath::FloatVector4    GetBlueNoiseContext() const                                           { return mRenderState->GetBlueNoiseContext(); }

    // RenderState
    const RenderState& GetRenderState() const { return *mRenderState; }
    RenderState* GetRenderStatePtr() const { return mRenderState; }

    void ResizeScreen(const LEMath::IntSize &size);

    // ===========================================
    // CTor & DTor
    // ===========================================
    DrawManager();
    virtual ~DrawManager();

private:            // Private members
    // Initialize frame buffers
    void initFrameBuffers();

    // Prepare for drawing model
    void prepareForDrawingModel();

    // Run tasks before flushing commands
    void runRendererTasks();

    // Calculation for getting jitter position using sample count (from msdn document for msaa)
    // https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_standard_multisample_quality_levels?redirectedfrom=MSDN
    LEMath::FloatPoint getJitterPosition(uint32 Index, uint32 NumOfSamples);

private:            // Private Properties
    uint32                               mFrameCounter;                     //!< FrameCounter

    CommandBuffer                       *mCommandBuffer;                    //!< Command buffer for rendering order
    RenderContext                       *mRenderContext;                    //!< Render context data
    RenderState                         *mRenderState;                      //!< Statement for rendering

    DrawManagerImpl                     *mImpl;                             //!< Implement of drawmanager
    Draw2DManager                       *mDraw2DManager;                    //!< Draw2D Manager

    Thread                              *mDrawThread;                       //!< Thread for drawing. (Run commandbuffer)
    bool                                 mDrawThreadExitCode;               //!< Exit flag for drawing thread
    Event                                mDrawEvent;                        //!< Event for flushing command buffer
    Event                                mReadyCommandBufferEvent;          //!< Event about ready commandbuffer

    Mutex                                mRendererTaskMutex;                //!< Mutex for running renderer tasks
    Mutex                                mCommandFlushMutex;                //!< Mutex for flushing commands
    VectorArray<RendererTask*>           mRendererTasks;                    //!< Task before flushing commands
                                                                            
    bool                                 mInitialized;                      //!< Is drawmanager initialized?
    bool                                 mSceneBegan;                       //!< Is scene began?
    bool                                 mModelDrawingBegan;                //!< Is drawing model began?
    bool                                 mReadyToRender;                    //!< Is it ready to render?
                                                                            
    LEMath::IntSize                      mScreenSize;                       //!< Current screen size
    LEMath::IntSize                      mRealScreenSize;                   //!< Current real screen size
                                         
    uint32                               mTemporalAASamples;                //!< Number of samples for TemporalAA

    FrameBufferTextureRefPtr             mLastFrameBufferTexture;           //!< Last frame buffer texture (for deferred release)
};    // DrawManager

class DrawManagerRendererAccessor
{
public:
    const explicit DrawManagerRendererAccessor(class DrawManager* manager)
        : mImpl(manager->mImpl)
        , mCommandBuffer(manager->mCommandBuffer)
    {}

    void* GetDeviceHandle() const { return mImpl ? mImpl->GetDeviceHandle() : nullptr; }
    void* GetDeviceContext() const { return mImpl ? mImpl->GetDeviceContext() : nullptr; }
    CommandBuffer* GetCommandBuffer() const { return mCommandBuffer; }

    void* GetImmediateCommandList() { return mImpl ? mImpl->GetImmediateCommandList() : nullptr; }
    uint64 ExecuteImmediateCommandList(void* cmdlist, bool waitcompletion) { return mImpl ? mImpl->ExecuteImmediateCommandList(cmdlist, waitcompletion) : nullptr; }

    void* AllocateGPUBuffer(size_t size) { return mImpl ? mImpl->AllocateGPUBuffer(size) : nullptr; }
    void* AllocateDescriptor(uint32 type) { return mImpl ? mImpl->AllocateDescriptor(type) : nullptr; }

    uint32 GetCurrentFrameBufferIndex() const { return mImpl ? mImpl->GetCurrentFrameBufferIndex() : 0u; }
    RendererFlag::BufferFormat GetCurrentFrameBufferFormat() const { return mImpl ? mImpl->GetCurrentFrameBufferFormat() : RendererFlag::BufferFormat::Unknown; }
    void* GetCurrentFrameBuffer() const { return mImpl ? mImpl->GetCurrentFrameBuffer() : nullptr; }
    void* GetCurrentFrameBufferView() const { return mImpl ? mImpl->GetCurrentFrameBufferView() : nullptr; }
    ResourceState GetCurrentFrameBufferResourceState() const { return mImpl ? mImpl->GetCurrentFrameBufferResourceState() : ResourceState::Common; }
    void SetCurrentFrameBufferResourceState(const ResourceState& state) { if (mImpl) mImpl->SetCurrentFrameBufferResourceState(state); }

    // FinishDrawing (executed by drawcommand)
    void Finalize(CommandBuffer* cmd) { if (mImpl) mImpl->Finalize(cmd); }

    // Present (executed by drawcommand)
    void Present() { if (mImpl) mImpl->Present(); }

private:
    DrawManagerImpl* mImpl = nullptr;
    CommandBuffer* mCommandBuffer = nullptr;
};

#define sDrawManager LimitEngine::DrawManager::GetSingletonPtr()
#define LE_DrawManager LimitEngine::DrawManager::GetSingleton()
#define LE_DrawManagerRendererAccessor DrawManagerRendererAccessor(LimitEngine::DrawManager::GetSingletonPtr())
}
