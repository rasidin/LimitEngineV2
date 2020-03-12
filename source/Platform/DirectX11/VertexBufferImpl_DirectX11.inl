/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  VertexBufferImpl_DirectX11.inl
@brief Vertex buffer for directx11
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifdef USE_DX11
#include <d3d11.h>

#include "Core/Debug.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/VertexBuffer.h"
#include "Managers/DrawManager.h"

namespace LimitEngine {
    class VertexBufferImpl_DirectX11 : public VertexBufferImpl
    {
    public:
        VertexBufferImpl_DirectX11()
            : mVertexBuffer(nullptr)
        {}
        virtual ~VertexBufferImpl_DirectX11()
        {
            Dispose();
        }

        void Create(uint32 fvf, size_t stride, size_t size, uint32 flag, void *buffer) override
        {
            ID3D11Device *handle = (ID3D11Device*)LE_DrawManager.GetDeviceHandle();
            LEASSERT(handle);

            size_t bufferSize = stride * size;
            D3D11_BUFFER_DESC bufferDesc;
            ::memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
            bufferDesc.ByteWidth = static_cast<UINT>(bufferSize);
            bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            if (flag & static_cast<uint32>(RendererFlag::CreateBufferFlags::CPU_WRITABLE))
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            if (flag & static_cast<uint32>(RendererFlag::CreateBufferFlags::CPU_READABLE))
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            if (flag & static_cast<uint32>(RendererFlag::CreateBufferFlags::CPU_WRITABLE))
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            bool madeDummyData = false;
            if (buffer == NULL) {
                buffer = malloc(bufferSize);
                ::memset(buffer, 0, bufferSize);
                madeDummyData = true;
            }

            D3D11_SUBRESOURCE_DATA subResData;
            ::memset(&subResData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
            subResData.pSysMem = buffer;
            HRESULT hr = handle->CreateBuffer(&bufferDesc, &subResData, &mVertexBuffer);
            if (FAILED(hr)) {
                Debug::Error("[VertexBuffer] Failed to create buffer");
            }

            if (madeDummyData) {
                free(buffer);
            }
        }
        void Dispose() override
        {
            if (mVertexBuffer)
            {
                mVertexBuffer->Release();
            }
            mVertexBuffer = NULL;
        }
        void Bind(Shader *shader) override { /* unimplemented */ }
        void* GetHandle() override { return mVertexBuffer; }
        void DrawPrimitive(uint32 type, size_t count) override { /* unimplemented */ }

    private:
        ID3D11Buffer            *mVertexBuffer;
    };
}
#endif