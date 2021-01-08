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

namespace LimitEngine {
class ConstantBufferImpl : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    ConstantBufferImpl() {}
    virtual ~ConstantBufferImpl() {}

    virtual void PrepareForDrawing() = 0;

    virtual void Set(uint32 ShaderType, uint32 BufferIndex, uint32 Offset, const void *Data, size_t Size) = 0;
};
class ConstantBuffer : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
public:
    ConstantBuffer();
    virtual ~ConstantBuffer();

    inline ConstantBufferImpl* GetImplementation() { return mImpl; }

    void PrepareForDrawing();
private:
    ConstantBufferImpl *mImpl;
};
}

#endif // LIMITENGINEV2_RENDERER_CONSTANTBUFFER_H_