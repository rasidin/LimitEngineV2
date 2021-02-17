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
@file  ConstantBuffer.h
@brief Constant buffer
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#ifndef LIMITENGINEV2_RENDERER_CONSTANTBUFFER_H_
#define LIMITENGINEV2_RENDERER_CONSTANTBUFFER_H_

#include <LERenderer>

#include "Core/Object.h"
#include "Core/ReferenceCountedObject.h"

namespace LimitEngine {
class ConstantBufferImpl : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    ConstantBufferImpl() {}
    virtual ~ConstantBufferImpl() {}

    virtual void Create(size_t size, void* initdata) = 0;
    virtual void Update(void* data) = 0;

    virtual void* GetResource() const = 0;
    virtual void* GetConstantBufferView() const = 0;
};
class ConstantBuffer : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
public:
    ConstantBuffer();
    virtual ~ConstantBuffer();

    void Create(size_t size, void* initdata);
    size_t GetSize() const { return mSize; }

private:
    ConstantBufferImpl *mImpl = nullptr;
    size_t mSize = 0u;

    friend class ConstantBufferRendererAccessor;
};
class ConstantBufferRendererAccessor
{
public:
    ConstantBufferRendererAccessor(ConstantBuffer* cb)
        : mImpl(cb->mImpl)
    {}
    void Update(void* data) { if (mImpl) mImpl->Update(data); }
    void* GetResource() const { return mImpl ? mImpl->GetResource() : nullptr; }
    void* GetConstantBufferView() const { return mImpl ? mImpl->GetConstantBufferView() : nullptr; }
private:
    ConstantBufferImpl* mImpl = nullptr;
};
}

#endif // LIMITENGINEV2_RENDERER_CONSTANTBUFFER_H_