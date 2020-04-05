/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  DrawManager.cpp
 @brief DrawManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Managers/DrawManager.h"
//#include "LE_SceneManager.h"
//#include "LE_PostFilterManager.h"

#include <LEIntVector2.h>

#if defined(USE_DX9)
#include "Platform/DirectX9/DrawManagerImpl_DirectX9.h"
#elif defined(USE_DX11)
#include "../Platform/DirectX11/DrawManagerImpl_DirectX11.inl"
#elif defined(USE_OPENGLES)
#include "Platform/OpenGLES/DrawManagerImpl_OpenGLES.h"
#else
#error No implementation DrawManagerImpl
#endif

//#include "LE_Camera.h"
//
#include "Core/Debug.h"
#include "Core/Timer.h"
#include "Managers/Draw2DManager.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/RenderContext.h"
//
//#include "LE_ShaderManager.h"
//#include "LE_LightManager.h"
//#include "LE_LightIBL.h"
//#include "LE_Texture.h"
//#include "LE_TaskManager.h"
//#include "LE_TaskPriority.h"
//#include "LE_Timer.h"
//#include "LE_FrameBuffer.h"
//
//#include "LE_PostFilter_Filmic.h"
//#include "LE_PostFilter_DOF.h"
//
//#include "LE_MemoryAllocator.h"
//#include "LE_RenderContext.h"

namespace LimitEngine
{
    DrawManager* SingletonDrawManager::mInstance = NULL;

    DrawManager::DrawManager()
        : SingletonDrawManager()
        , mCommandBuffer(nullptr)
        , mRenderContext(nullptr)
        , mRenderState(nullptr)
        , mImpl(nullptr)
        , mDraw2DManager(nullptr)
        , mDrawThread(nullptr)
        , mDrawThreadExitCode(false)
        , mDrawEvent("DrawEvent")
        , mReadyCommandBufferEvent("ReadyCommandBuffer")
        , mCommandFlushMutex()
        , mInitialized(false)
        , mSceneBegan(false)
        , mModelDrawingBegan(false)
        , mReadyToRender(false)
        , mScreenSize(0, 0)
    {
        mCommandBuffer = new CommandBuffer();
        mRenderState = new RenderState();
        mDraw2DManager = new Draw2DManager();

        mDrawThread = new Thread();
    }

    DrawManager::~DrawManager()
    {
        if (mDrawThread) {
            mDrawThreadExitCode = true;
            FlushCommand();
            WaitForFlushing();
            if(mDrawThread->Join()) {
                delete mDrawThread;
            }
            mDrawThread = NULL;
        }

        if (mCommandBuffer) {
            mCommandBuffer->Term();
            delete mCommandBuffer;
        }
		mCommandBuffer = nullptr;
		if (mRenderState) delete mRenderState;
        mRenderState = nullptr;
		if (mRenderContext) delete mRenderContext;
		mRenderContext = nullptr;

		if (mImpl)
        {
            mImpl->Term();
            delete mImpl;
        }
        if (mDraw2DManager)
        {
            mDraw2DManager->Term();
            delete mDraw2DManager;
        }
    }

    void DrawManager::Init(WINDOW_HANDLE handle)
    {
#if defined(USE_DX9)
        mImpl = new DrawManagerImpl_DirectX9();
#elif defined(USE_DX11)
        mImpl = new DrawManagerImpl_DirectX11();
#elif defined(USE_OPENGLES)
        mImpl = new DrawManagerImpl_OpenGLES();
#endif
        mRenderContext = new RenderContext();

        mImpl->Init(handle);
        mDraw2DManager->Init();

        AutoPointer<void> InitParameter = mImpl->MakeInitParameter();
        mCommandBuffer->Init(InitParameter);

        ThreadParam threadParameter;
        threadParameter.func = ThreadFunction(this, &DrawManager::FlushCommandThread);
        mDrawThread->Create(threadParameter);
        
        mInitialized = true;
    }

    void DrawManager::Run()
    {
        if (mInitialized == false)
            return;

        LEASSERT(mImpl);
        // Initialize before rendering...(once)
        if (!mReadyToRender)
        {
            initFrameBuffers();
        }
        // Ready to render
        mImpl->ReadyToRender();

        // Wait for frame
        WaitForFlushing();

        // Present screen
        mImpl->Present(NULL);

        runRendererTasks();
        // Run drawing commands
        FlushCommand();
    }

    void DrawManager::ResizeScreen(const LEMath::IntSize &size)
    {
        LEASSERT(mImpl);
        //if(mSceneBegan) {
        //    mImpl->EndScene();
        //}
        mImpl->ResizeScreen(size);
        //if(mSceneBegan) {
        //    mImpl->BeginScene();
        //}
    }
    void DrawManager::FlushCommandThread()
    {
        static constexpr double frameLimitTime = 1.0/60.0; // 16ms (60fps)

        double lastTime = Timer::GetTimeDoubleSecond();
        while (1)
        {
            if (mDrawThreadExitCode)
                break;
            mDrawEvent.Wait();
            mReadyCommandBufferEvent.Signal();

            mCommandBuffer->LockWriteCommandBuffer();
            Mutex::ScopedLock scopedLock(mCommandFlushMutex);
            if (mCommandBuffer) {
                mCommandBuffer->Flush(mRenderState);
            }
            double time = Timer::GetTimeDoubleSecond();
            double elapsedTime = time - lastTime;
#ifdef WINDOWS
            DWORD sleepTime = 0u;
            if (elapsedTime < frameLimitTime)
                sleepTime = static_cast<DWORD>((frameLimitTime - elapsedTime) * 1000);
            if (sleepTime)
                Sleep(sleepTime);
#endif
            lastTime = time;
        }
    }
    void DrawManager::runRendererTasks()
    {
        for (uint32 rtidx = 0; rtidx < mRendererTasks.size(); rtidx++) {
            mRendererTasks[rtidx]->Run();
            delete mRendererTasks[rtidx];
        }
        mRendererTasks.Clear();
    }
    // Run all of drawing commands
    void DrawManager::FlushCommand()
    {
        // Run commandbuffer
        mDrawEvent.Signal();

        mDraw2DManager->EndOfFrame();

        // Sync flushcommandthread
        mReadyCommandBufferEvent.Wait();
    }
    void DrawManager::WaitForFlushing()
    {
        Mutex::ScopedLock scopedLock(mCommandFlushMutex);
    }
    // Update matrices for rendering scene
    void DrawManager::UpdateMatrices()
    {
        LEMath::FloatMatrix4x4 viewMatrix = mRenderState->GetViewMatrix();
        LEMath::FloatMatrix4x4 projMatrix = mRenderState->GetProjMatrix();
        LEMath::FloatMatrix4x4 viewProjMatrix = projMatrix * viewMatrix;

        mRenderState->SetViewProjMatrix(viewProjMatrix);
        mRenderState->SetInvViewMatrix(viewMatrix.Inverse());
        mRenderState->SetInvProjMatrix(projMatrix.Inverse());
        mRenderState->SetInvViewProjMatrix(viewProjMatrix.Inverse());
    }
    // Initialize before rendering scene
    void DrawManager::initFrameBuffers()
    {
        LEMath::IntSize screenSize = mImpl->GetScreenSize();
        if (screenSize.Width() <= 0 || screenSize.Height() <= 0) return;

        mScreenSize = ConvertReal2Virtual(screenSize);
        //SceneManager::GetSingleton().GetCamera()->SetScreenSize(mScreen_size.width, mScreen_size.height);
        //
        //PostFilterManager::GetSingleton().Init();
        mReadyToRender = true;
    }
    // Prepare for drawing scene
    void DrawManager::PrepareForDrawingScene()
    {
        //if(LightIBL *ibl = LE_LightManager.GetStandardIBLLight()) {
        //    mRenderState->SetBRDFLUT(ibl->GetBRDFLUTTexture());
        //    mRenderState->SetIBLDiffuseTexture(ibl->GetIBLDiffuseTexture());
        //    mRenderState->SetIBLSpecularTexture(ibl->GetIBLSpecularTexture());
        //}
    }
    // Prepare for drawing model
    void DrawManager::prepareForDrawingModel()
    {
        //if (mImpl) {
        //    mImpl->PrepareForDrawingModel();
        //}
    }
}