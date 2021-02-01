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
@file  ConstantBuffer.cpp
@brief Constant buffer
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include "Renderer/ConstantBuffer.h"
#include "Managers/DrawManager.h"

#if defined(USE_DX12)
#include "../Platform/DirectX12/ConstantBufferImpl_DirectX12.inl"
#else
#error No implementation of constant buffer
#endif

namespace LimitEngine {
class RendererTask_CreateConstantBuffer : public RendererTask
{
public:
    RendererTask_CreateConstantBuffer(ConstantBufferImpl* owner, size_t size, void* initdata)
        : mOwner(owner)
        , mSize(size)
        , mInitData(initdata)
    {}
    void Run() override
    {
        if (mOwner)
            mOwner->Create(mSize, mInitData);
    }
private:
    ConstantBufferImpl* mOwner;
    size_t mSize;
    void* mInitData;
};
ConstantBuffer::ConstantBuffer()
    : mImpl(nullptr)
    , mSize(0u)
{
#if defined(USE_DX12)
    mImpl = new ConstantBufferImpl_DirectX12();
#else
#error No implementation of ConstantBuffer for this platform!
#endif
}
ConstantBuffer::~ConstantBuffer()
{
    if (mImpl) {
        delete mImpl;
        mImpl = nullptr;
    }
}
void ConstantBuffer::Create(size_t size, void* initdata)
{
    mSize = size;
    AutoPointer<RendererTask> rt_createconstantbuffer = new RendererTask_CreateConstantBuffer(mImpl, size, initdata);
    LE_DrawManager.AddRendererTask(rt_createconstantbuffer);
}
} // namespace LimitEngine