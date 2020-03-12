/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  DrawManagerImpl_DirectX11.inl
@brief Main File for Windows
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifdef USE_DX11
#include <d3d11.h>

#include <LERenderer>

#include "Core/Debug.h"
#include "Containers/MapArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#include "PrivateDefinitions_DirectX11.h"

namespace LimitEngine {
    class DrawManagerImpl_DirectX11 : public DrawManagerImpl
    {
    public:
        DrawManagerImpl_DirectX11()
            : mD3DDevice(nullptr)
            , mD3DDeviceContext(nullptr)
            , mDXGIDevice(nullptr)
            , mDXGIAdapter(nullptr)
            , mDXGIFactory(nullptr)
            , mDXGISwapChain(nullptr)
            , mBaseRenderTarget(nullptr)
            , mBaseDepthStencilTexture(nullptr)
            , mBaseRenderTargetView(nullptr)
            , mBaseDepthStencilView(nullptr)
        {
        }

        virtual ~DrawManagerImpl_DirectX11() {}

        void ReadyToRender() override
        {
            mD3DDeviceContext->OMSetRenderTargets(1, &mBaseRenderTargetView, mBaseDepthStencilView);
        }
        LEMath::IntSize GetScreenSize() override
        {
            if (!mD3DDevice) return LEMath::IntSize();
            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            if (mDXGISwapChain->GetDesc(&swapChainDesc) == S_OK) {
                return LEMath::IntSize(swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);
            }
            return LEMath::IntSize();
        }
        void PreRenderFinished() override {}

        AutoPointer<void> MakeInitParameter() {
            CommandInit_Parameter *Output = new CommandInit_Parameter();

            Output->mD3DDevice = mD3DDevice;
            Output->mD3DDeviceContext = mD3DDeviceContext;
            Output->mBaseRenderTargetView = mBaseRenderTargetView;
            Output->mBaseDepthStencilView = mBaseDepthStencilView;

            return AutoPointer<void>((void*)Output);
        }

        void Init(WINDOW_HANDLE handle) override
        {
            const D3D_FEATURE_LEVEL featureLevels[] =
            {
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1,
            };
            const UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
            // Create device
            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            ::memset(&swapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
            swapChainDesc.Windowed = true;
            swapChainDesc.BufferCount = 1;
            swapChainDesc.BufferDesc.Width = 1920;
            swapChainDesc.BufferDesc.Height = 1080;
            swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = handle;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            D3D_FEATURE_LEVEL validFeatureLevel;
            if (FAILED(D3D11CreateDeviceAndSwapChain(mDXGIAdapter, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, &mDXGISwapChain, &mD3DDevice, &validFeatureLevel, &mD3DDeviceContext))) {
                Debug::Error("Failed to create device");
                return;
            }
            // Get Backbuffer
            if (FAILED(mDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBaseRenderTarget))) {
                Debug::Error("Failed to get backbuffer");
                return;
            }
            // Create RenderTargetView
            if (FAILED(mD3DDevice->CreateRenderTargetView(mBaseRenderTarget, NULL, &mBaseRenderTargetView))) {
                Debug::Error("Failed to create rendertargetview");
                return;
            }
            // Create DepthStencilView
            D3D11_TEXTURE2D_DESC depthStencilDesc;
            ::memset(&depthStencilDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
            depthStencilDesc.Width = 1920;
            depthStencilDesc.Height = 1080;
            depthStencilDesc.MipLevels = 1;
            depthStencilDesc.ArraySize = 1;
            depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
            depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
            depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            if (FAILED(mD3DDevice->CreateTexture2D(&depthStencilDesc, NULL, &mBaseDepthStencilTexture))) {
                Debug::Error("Failed to create depthstencil texture");
                return;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
            ::memset(&depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
            depthStencilViewDesc.Format = depthStencilDesc.Format;
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            if (FAILED(mD3DDevice->CreateDepthStencilView(mBaseDepthStencilTexture, &depthStencilViewDesc, &mBaseDepthStencilView))) {
                Debug::Error("Failed to create depthstencilview");
                return;
            }
            // Set viewport
            D3D11_VIEWPORT viewPort;
            viewPort.TopLeftX = 0;
            viewPort.TopLeftY = 0;
            viewPort.Width = 1920;
            viewPort.Height = 1080;
            viewPort.MinDepth = 0.0f;
            viewPort.MaxDepth = 1.0f;
            mD3DDeviceContext->RSSetViewports(1, &viewPort);
        }
        void ResizeScreen(const LEMath::IntSize &size) override {}
        void Present(Texture *texture) override
        {
            if (mDXGISwapChain) {
                HRESULT presentResult = mDXGISwapChain->Present(0, 0);
                if (FAILED(presentResult)) {
                    Debug::Error("Failed to present!!");
                }
            }
        }
        void Term() override
        {
            if (mBaseRenderTargetView != NULL) {
                mBaseRenderTargetView->Release();
                mBaseRenderTargetView = NULL;
            }
            if (mBaseDepthStencilView != NULL) {
                mBaseDepthStencilView->Release();
                mBaseDepthStencilView = NULL;
            }
            if (mBaseRenderTarget != NULL) {
                mBaseRenderTarget->Release();
                mBaseRenderTarget = NULL;
            }
            if (mDXGISwapChain != NULL) {
                mDXGISwapChain->Release();
                mDXGISwapChain = NULL;
            }
            if (mD3DDeviceContext != NULL) {
                mD3DDeviceContext->Release();
                mD3DDeviceContext = NULL;
            }
            if (mD3DDevice != NULL) {
                mD3DDevice->Release();
                mD3DDevice = NULL;
            }
        }
        void BindFrameBuffer(uint32 buffer) override {}

        void* GetDeviceHandle() const override { return mD3DDevice; }
        void* GetDeviceContext() const override { return mD3DDeviceContext; }

    private:
        ID3D11Device			*mD3DDevice;
        ID3D11DeviceContext		*mD3DDeviceContext;
        IDXGIDevice1			*mDXGIDevice;
        IDXGIAdapter			*mDXGIAdapter;
        IDXGIFactory			*mDXGIFactory;
        IDXGISwapChain			*mDXGISwapChain;

        ID3D11Texture2D			*mBaseRenderTarget;                             // Texture for rendertarget
        ID3D11Texture2D         *mBaseDepthStencilTexture;                      // Texture for depthstencil
        ID3D11RenderTargetView	*mBaseRenderTargetView;                         // View for rendertarget
        ID3D11DepthStencilView	*mBaseDepthStencilView;                         // View for depthstencil
    };
}

#endif