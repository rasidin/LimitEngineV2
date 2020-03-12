/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  IndexBufferImpl_DirectX11.cpp
@brief Index buffer for directx11
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifdef USE_DX11
#include <d3d11.h>

#include "Core/Debug.h"
#include "Managers/DrawManager.h"

namespace LimitEngine {
class IndexBufferImpl_DirectX11 : public IndexBufferImpl 
{public:
    IndexBufferImpl_DirectX11()
        : IndexBufferImpl()
        , mIndexBuffer(nullptr)
    {}
    virtual ~IndexBufferImpl_DirectX11()
    {
    }

    virtual void Create(size_t size, void *buffer) override
    {
        size_t bufferSize = size * sizeof(uint32);

        ID3D11Device *device = (ID3D11Device*)LE_DrawManager.GetDeviceHandle();
        LEASSERT(device);

        D3D11_BUFFER_DESC bufferDesc;
        ::memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(bufferSize);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA resData;
        ::memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
        resData.pSysMem = buffer;
        resData.SysMemPitch = 0;
        resData.SysMemSlicePitch = 0;
        if (device->CreateBuffer(&bufferDesc, &resData, &mIndexBuffer) != S_OK) {
            Debug::Warning("[IndexBuffer]Failed to create index buffer");
        }
    }
    virtual void Dispose() override
    {
        if (mIndexBuffer) {
            mIndexBuffer->Release();
        }
        mIndexBuffer = NULL;
    }
    virtual void* GetHandle() override
    {
        return mIndexBuffer;
    }
    virtual void* GetBuffer() override
    { // unimplemented
        return NULL;
    }
private:
    ID3D11Buffer        *mIndexBuffer;
};
}
#endif