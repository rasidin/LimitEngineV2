/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  DrawManager.cpp
 @brief DrawManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Managers/DrawManager.h"

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
        , mFrameCounter(0u)
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
        , mTemporalAASamples(16u)
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

    void DrawManager::Init(WINDOW_HANDLE handle, const InitializeOptions &Options)
    {
#if defined(USE_DX9)
        mImpl = new DrawManagerImpl_DirectX9();
#elif defined(USE_DX11)
        mImpl = new DrawManagerImpl_DirectX11();
#elif defined(USE_OPENGLES)
        mImpl = new DrawManagerImpl_OpenGLES();
#endif
        mRenderContext = new RenderContext();

        mImpl->Init(handle, Options);
        mDraw2DManager->Init();

        mCommandBuffer->Init(mImpl->MakeInitParameter());

        ThreadParam threadParameter;
        threadParameter.func = ThreadFunction(this, &DrawManager::FlushCommandThread);
        threadParameter.name = "DrawCommand";
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
            if (mDrawThreadExitCode) {
                mReadyCommandBufferEvent.Signal();
                break;
            }
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
    LEMath::FloatPoint DrawManager::getJitterPosition(uint32 Index, uint32 NumOfSamples)
    {
        switch (NumOfSamples)
        {
        case 1:
            return LEMath::FloatPoint(0, 0);
        case 2: {
            return (Index == 0)?LEMath::FloatPoint( 4.0f/8.0f, 4.0f/8.0f):LEMath::FloatPoint(-4.0f/8.0f,-4.0f/8.0f);
        }
        case 4: {
            static constexpr float SampleX[] = {-2.0f/8.0f, 6.0f/8.0f,-6.0f/8.0f, 2.0f/8.0f };
            static constexpr float SampleY[] = {-6.0f/8.0f,-2.0f/8.0f, 2.0f/8.0f, 6.0f/8.0f };
            return LEMath::FloatPoint(SampleX[Index], SampleY[Index]);
        }
        case 8: {
            static constexpr float SampleX[] = { 1.0f / 8.0f,-1.0f / 8.0f, 5.0f / 8.0f,-3.0f / 8.0f,-5.0f / 8.0f,-7.0f / 8.0f, 3.0f / 8.0f, 7.0f / 8.0f };
            static constexpr float SampleY[] = {-3.0f / 8.0f, 3.0f / 8.0f, 1.0f / 8.0f,-5.0f / 8.0f, 5.0f / 8.0f,-1.0f / 8.0f, 7.0f / 8.0f,-7.0f / 8.0f };
            return LEMath::FloatPoint(SampleX[Index], SampleY[Index]);
        }
        case 16: {
            static constexpr float SampleX[] = { 1.0f / 8.0f,-1.0f / 8.0f,-3.0f / 8.0f,-4.0f / 8.0f,-5.0f / 8.0f, 2.0f / 8.0f, 5.0f / 8.0f, 3.0f / 8.0f,-2.0f / 8.0f, 0.0f / 8.0f,-4.0f / 8.0f,-6.0f / 8.0f,-8.0f / 8.0f, 7.0f / 8.0f, 6.0f / 8.0f,-7.0f / 8.0f };
            static constexpr float SampleY[] = { 1.0f / 8.0f,-3.0f / 8.0f, 2.0f / 8.0f,-1.0f / 8.0f,-2.0f / 8.0f, 5.0f / 8.0f, 3.0f / 8.0f,-5.0f / 8.0f, 6.0f / 8.0f,-7.0f / 8.0f,-6.0f / 8.0f, 4.0f / 8.0f,-8.0f / 8.0f,-4.0f / 8.0f, 7.0f / 8.0f,-8.0f / 8.0f };
            return LEMath::FloatPoint(SampleX[Index], SampleY[Index]);
        }
        default:
            LEASSERT(0);
            break;
        }
        return LEMath::FloatPoint::Zero;
    }
    // Update matrices for rendering scene
    void DrawManager::UpdateMatrices()
    {
        uint32 temporalAAIndex = mFrameCounter % mTemporalAASamples;
        LEMath::FloatPoint jitter = getJitterPosition(temporalAAIndex, mTemporalAASamples);

        LEMath::FloatMatrix4x4 viewMatrix = mRenderState->GetViewMatrix();
        LEMath::FloatMatrix4x4 projMatrix = mRenderState->GetProjMatrix();
        LEMath::FloatMatrix4x4 projMatrixWithJitter = projMatrix;
        if (mRealScreenSize.Size()) {
            projMatrixWithJitter.Set(2, 0, jitter.X() / mRealScreenSize.Width());
            projMatrixWithJitter.Set(2, 1, jitter.Y() / mRealScreenSize.Height());
        }
        LEMath::FloatMatrix4x4 viewProjMatrix = viewMatrix * projMatrixWithJitter;

        mRenderState->SetViewProjMatrix(viewProjMatrix.Transpose());
        mRenderState->SetInvViewMatrix(viewMatrix.Inverse());
        mRenderState->SetInvProjMatrix(projMatrixWithJitter.Inverse());
        mRenderState->SetInvViewProjMatrix(viewProjMatrix.Inverse());

        mRenderState->SetTemporalContext(
            //temporalAAIndex, mTemporalAASamples, 32, 32 * mTemporalAASamples
            0, 1, 128, 128
        );

        mFrameCounter++;
    }
    // Initialize before rendering scene
    void DrawManager::initFrameBuffers()
    {
        mRealScreenSize = mImpl->GetScreenSize();
        if (mRealScreenSize.Width() <= 0 || mRealScreenSize.Height() <= 0) return;

        mScreenSize = ConvertReal2Virtual(mRealScreenSize);
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