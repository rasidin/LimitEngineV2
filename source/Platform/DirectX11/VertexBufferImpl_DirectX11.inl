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
---------------------------------------------------------------------
@file  VertexBufferImpl_DirectX11.inl
@brief VertexBuffer (DirectX11)
@author minseob (leeminseob@outlook.com)
*********************************************************************/
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