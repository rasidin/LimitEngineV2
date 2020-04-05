/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureImpl_DirectX11.inl
@brief Texture (DirectX11)
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifdef USE_DX11
#include <d3d11.h>

#include <LEIntVector2.h>
#include <LEIntVector3.h>

#include "Core/Debug.h"
#include "Managers/DrawManager.h"

namespace LimitEngine {
    // Utilities
    DXGI_FORMAT ConvertTextureFormat(const TEXTURE_COLOR_FORMAT &format)
    {
        switch (format)
        {
        case TEXTURE_COLOR_FORMAT_R8G8B8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TEXTURE_COLOR_FORMAT_A8R8G8B8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TEXTURE_COLOR_FORMAT_R16F:
            return DXGI_FORMAT_R16_FLOAT;
        case TEXTURE_COLOR_FORMAT_G16R16F:
            return DXGI_FORMAT_R16G16_FLOAT;
        case TEXTURE_COLOR_FORMAT_A16B16G16R16F:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case TEXTURE_COLOR_FORMAT_R32F:
            return DXGI_FORMAT_R32_FLOAT;
        case TEXTURE_COLOR_FORMAT_G32R32F:
            return DXGI_FORMAT_R32G32_FLOAT;
        case TEXTURE_COLOR_FORMAT_A32B32G32R32F:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }
    UINT CalculatePitchSize(const TEXTURE_COLOR_FORMAT &format, LEMath::IntSize size)
    {
        UINT pixelSize = size.Width();
        switch (format)
        {
        case TEXTURE_COLOR_FORMAT_R8G8B8:
            return pixelSize * sizeof(uint8) * 3;
        case TEXTURE_COLOR_FORMAT_A8R8G8B8:
            return pixelSize * sizeof(uint32);
        case TEXTURE_COLOR_FORMAT_R16F:
            return pixelSize * sizeof(float) * 1 / 2;
        case TEXTURE_COLOR_FORMAT_G16R16F:
            return pixelSize * sizeof(float) * 2 / 2;
        case TEXTURE_COLOR_FORMAT_A16B16G16R16F:
            return pixelSize * sizeof(float) * 4 / 2;
        case TEXTURE_COLOR_FORMAT_R32F:
            return pixelSize * sizeof(float) * 1;
        case TEXTURE_COLOR_FORMAT_G32R32F:
            return pixelSize * sizeof(float) * 2;
        case TEXTURE_COLOR_FORMAT_A32B32G32R32F:
            return pixelSize * sizeof(float) * 4;
        default:
            return 0u;
        }
        return 0u;
    }
    UINT CalculateSlideSize(const TEXTURE_COLOR_FORMAT &format, LEMath::IntSize size)
    {
        UINT pixelSize = size.Width() * size.Height();
        switch (format)
        {
        case TEXTURE_COLOR_FORMAT_R8G8B8:
            return pixelSize * sizeof(uint8) * 3;
        case TEXTURE_COLOR_FORMAT_A8R8G8B8:
            return pixelSize * sizeof(uint32);
        case TEXTURE_COLOR_FORMAT_R16F:
            return pixelSize * sizeof(float) * 1 / 2;
        case TEXTURE_COLOR_FORMAT_G16R16F:
            return pixelSize * sizeof(float) * 2 / 2;
        case TEXTURE_COLOR_FORMAT_A16B16G16R16F:
            return pixelSize * sizeof(float) * 4 / 2;
        case TEXTURE_COLOR_FORMAT_R32F:
            return pixelSize * sizeof(float) * 1;
        case TEXTURE_COLOR_FORMAT_G32R32F:
            return pixelSize * sizeof(float) * 2;
        case TEXTURE_COLOR_FORMAT_A32B32G32R32F:
            return pixelSize * sizeof(float) * 4;
        default:
            return 0u;
        }
        return 0u;
    }

    class TextureImpl_DirectX11 : public TextureImpl
    {
    public:
        TextureImpl_DirectX11()
            : mTexture2D(nullptr)
            , mTexture3D(nullptr)
            , mShaderResourceView(nullptr)
            , mUnorderedAccessView(nullptr)
            , mSize()
            , mDepth(0)
            , mFormat()
        {}
        virtual ~TextureImpl_DirectX11()
        {
            if (mShaderResourceView) {
                mShaderResourceView->Release();
            }
            if (mUnorderedAccessView) {
                mUnorderedAccessView->Release();
            }
            mShaderResourceView = nullptr;
        }

        LEMath::IntSize GetSize() override { return mSize; }
        TEXTURE_COLOR_FORMAT GetFormat() override { return mFormat; }
        void* GetHandle() override { return mShaderResourceView; }
        void* GetDepthSurfaceHandle() override { return nullptr; }

        void Load(const char *filename)
        {
            /* unimplemented */
        }
        void LoadFromMemory(const void *data, size_t size) override
        {
            if (mShaderResourceView) mShaderResourceView->Release(); mShaderResourceView = NULL;
            ID3D11Device *device = (ID3D11Device*)sDrawManager->GetDeviceHandle();
            LEASSERT(device);
            //HRESULT hrLoadTexture;
            //D3DX11_IMAGE_LOAD_INFO imageLoadInfo;
            //if (D3DX11CreateShaderResourceViewFromMemory(device, data, size, NULL, NULL, &mShaderResourceView, &hrLoadTexture) == S_OK) {
            //    // Get texture size (unimplemented)
            //}
            //else {
            //    Debug::Error("[TextureImpl_DirectX11]Failed to load texture from memory[%x] : %d", (uintptr_t)data, hrLoadTexture);
            //}
        }
        bool Create(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initializeData, size_t initDataSize) override
        {
            if (mShaderResourceView) mShaderResourceView->Release(); mShaderResourceView = NULL;
            if (mUnorderedAccessView) mUnorderedAccessView->Release(); mUnorderedAccessView = NULL;

            ID3D11Device *device = (ID3D11Device*)LE_DrawManager.GetDeviceHandle();
            LEASSERT(device);

            D3D11_TEXTURE2D_DESC tex2DDesc;
            ::memset(&tex2DDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
            tex2DDesc.Width = size.Width();
            tex2DDesc.Height = size.Height();
            tex2DDesc.MipLevels = mipLevels;
            tex2DDesc.Format = ConvertTextureFormat(format);
            tex2DDesc.SampleDesc.Count = 1;
            tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
            tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
            tex2DDesc.ArraySize = 1;

            D3D11_SUBRESOURCE_DATA tex2DSubResource;
            if (initializeData) {
                ::memset(&tex2DSubResource, 0, sizeof(D3D11_SUBRESOURCE_DATA));
                tex2DSubResource.pSysMem = initializeData;
                tex2DSubResource.SysMemPitch = CalculatePitchSize(format, size);
                tex2DSubResource.SysMemSlicePitch = CalculateSlideSize(format, size);
            }

            if (device->CreateTexture2D(&tex2DDesc, initializeData?(&tex2DSubResource):NULL, &mTexture2D) == S_OK) {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

                ::memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
                srvDesc.Format = tex2DDesc.Format;
                srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = 1;
                srvDesc.Texture2D.MostDetailedMip = 0;
                if (device->CreateShaderResourceView(mTexture2D, &srvDesc, &mShaderResourceView) != S_OK) {
                    Debug::Error("[TextureImpl_DirectX11] Failed to create shader resource view");
                    return false;
                }
                mFormat = format;
                mSize = size;

                D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
                ::memset(&uavDesc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
                uavDesc.Format = tex2DDesc.Format;
                uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
                if (device->CreateUnorderedAccessView(mTexture2D, &uavDesc, &mUnorderedAccessView) != S_OK) {
                    Debug::Error("[TextureImpl_DirectX11] Failed to create unordered access view");
                    return false;
                }
                return true;
            }
            return false;
        }
        bool Create3D(const LEMath::IntSize3 &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initializeData, size_t initDataSize) override
        {
            if (mShaderResourceView) mShaderResourceView->Release(); mShaderResourceView = NULL;
            if (mUnorderedAccessView) mUnorderedAccessView->Release(); mUnorderedAccessView = NULL;

            ID3D11Device *device = (ID3D11Device*)LE_DrawManager.GetDeviceHandle();
            LEASSERT(device);

            D3D11_TEXTURE3D_DESC tex3DDesc;
            ::memset(&tex3DDesc, 0, sizeof(D3D11_TEXTURE3D_DESC));
            tex3DDesc.Width = size.Width();
            tex3DDesc.Height = size.Height();
            tex3DDesc.Depth = size.Depth();
            tex3DDesc.MipLevels = mipLevels;
            tex3DDesc.Format = ConvertTextureFormat(format);
            tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
            tex3DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

            D3D11_SUBRESOURCE_DATA tex3DSubRes;
            ::memset(&tex3DSubRes, 0, sizeof(D3D11_SUBRESOURCE_DATA));
            tex3DSubRes.pSysMem = initializeData;
            tex3DSubRes.SysMemPitch = CalculatePitchSize(format, size.XY());
            tex3DSubRes.SysMemSlicePitch = CalculateSlideSize(format, size.XY());
            if (device->CreateTexture3D(&tex3DDesc, initializeData ? (&tex3DSubRes) : NULL, &mTexture3D) == S_OK) {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

                ::memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
                srvDesc.Format = tex3DDesc.Format;
                srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
                srvDesc.Texture3D.MipLevels = mipLevels;
                srvDesc.Texture3D.MostDetailedMip = 0;
                if (device->CreateShaderResourceView(mTexture3D, &srvDesc, &mShaderResourceView) != S_OK) {
                    Debug::Error("[TextureImpl_DirectX11] Failed to create shader resource view");
                    return false;
                }
                mFormat = format;
                mSize = size.XY();
                mDepth = size.Depth();

                D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
                ::memset(&uavDesc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
                uavDesc.Format = tex3DDesc.Format;
                uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
                uavDesc.Texture3D.MipSlice = 0;
                uavDesc.Texture3D.FirstWSlice = 0;
                uavDesc.Texture3D.WSize = size.Depth();
                if (device->CreateUnorderedAccessView(mTexture3D, &uavDesc, &mUnorderedAccessView) != S_OK) {
                    Debug::Error("[TextureImpl_DirectX11] Failed to create unordered access view");
                    return false;
                }
                return true;
            }
            return false;
        }
        void CreateScreenColor(const LEMath::IntSize &size) override { /* unimplemented */ }
        void CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color) override
        {
            if (mShaderResourceView) mShaderResourceView->Release(); mShaderResourceView = NULL;

            ID3D11Device *device = (ID3D11Device*)LE_DrawManager.GetDeviceHandle();
            LEASSERT(device);

            D3D11_TEXTURE2D_DESC tex2DDesc;
            ::memset(&tex2DDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
            tex2DDesc.Width = size.Width();
            tex2DDesc.Height = size.Height();
            tex2DDesc.MipLevels = 1;
            tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            tex2DDesc.SampleDesc.Count = 1;
            tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
            tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            tex2DDesc.ArraySize = 1;

            uint32 *colorBuffer = new uint32[size.Size()]();
            for (int colidx = 0; colidx < size.Size(); colidx++) {
                colorBuffer[colidx] = color.m;
            }
            D3D11_SUBRESOURCE_DATA srsData;
            ::memset(&srsData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
            srsData.pSysMem = colorBuffer;
            srsData.SysMemPitch = size.Width() * 4;
            srsData.SysMemSlicePitch = size.Size() * 4;
            if (device->CreateTexture2D(&tex2DDesc, &srsData, &mTexture2D) == S_OK) {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                ::memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
                srvDesc.Format = tex2DDesc.Format;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MostDetailedMip = 0;
                srvDesc.Texture2D.MipLevels = 1;

                if (device->CreateShaderResourceView(mTexture2D, &srvDesc, &mShaderResourceView) == S_OK) {
                    mFormat = TEXTURE_COLOR_FORMAT_A8R8G8B8;
                    mSize = size;
                }
            }

            delete[] colorBuffer;
        }
        void CreateDepth(const LEMath::IntSize &size) override { /* unimplemented */ }
        void GenerateMipmap() override { /* unimplemented */ }

        void* Lock(const LEMath::IntRect &rect, int mipLevel = 0) override { return nullptr; /* unimplemented */ }
        void Unlock(int mipLevel = 0) override { /* unimplemented */ }

        void* GetShaderResourceView() const override { return mShaderResourceView; }
        void* GetUnorderedAccessView() const override { return mUnorderedAccessView; }

    private:
        ID3D11Texture2D                      *mTexture2D;
        ID3D11Texture3D						 *mTexture3D;
        ID3D11ShaderResourceView             *mShaderResourceView;
        ID3D11UnorderedAccessView			 *mUnorderedAccessView;
        LEMath::IntSize                       mSize;
        uint32								  mDepth;
        TEXTURE_COLOR_FORMAT                  mFormat;
    };
}
#endif