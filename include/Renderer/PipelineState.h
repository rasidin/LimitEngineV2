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
@file  PipelineState.h
@brief PipelineState
@author minseob (https://github.com/rasidin)
**********************************************************************/
#ifndef LIMITENGINEV2_PIPELINESTATE_H_
#define LIMITENGINEV2_PIPELINESTATE_H_

#include <LERenderer>

#include "Core/Memory.h"
#include "Core/ReferenceCountedObject.h"
#include "Renderer/PipelineStateDescriptor.h"

namespace LimitEngine {
class PipelineStateImpl
{public:
    virtual bool  IsValid() const = 0;
    virtual bool  Init(const PipelineStateDescriptor& desc) = 0;
    virtual void* GetRootSignatureHandle() const = 0;
    virtual void* GetHandle() const = 0;
};

class PipelineStateRendererAccessor
{
public:
    explicit PipelineStateRendererAccessor(const class PipelineState* state);

    void* GetRootSignatureHandle() const { return mImpl ? mImpl->GetRootSignatureHandle() : nullptr; }
    void* GetHandle() const { return mImpl ? mImpl->GetHandle() : nullptr; }

private:
    PipelineStateImpl* mImpl = nullptr;
};

class PipelineState : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
public:
    PipelineState();
    virtual ~PipelineState() {}

    bool IsValid() const { return mImpl->IsValid(); }
    bool Init(const PipelineStateDescriptor& desc) { return mImpl->Init(desc); }

private:
    PipelineStateImpl*      mImpl;

    friend PipelineStateRendererAccessor;
};
} // namespace LimitEngine

#endif // LIMITENGINEV2_PIPELINESTATE_H_