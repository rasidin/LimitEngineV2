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
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#ifndef LIMITENGINEV2_PIPELINESTATE_H_
#define LIMITENGINEV2_PIPELINESTATE_H_

#include <LERenderer>

#include "Core/Memory.h"
#include "Core/ReferenceCountedObject.h"
#include "Renderer/PipelineStateDescriptor.h"

namespace LimitEngine {
class PipelineStateImpl
{
    virtual bool Init(const PipelineStateDescriptor& desc) = 0;
};

class PipelineState : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
public:
    PipelineState(const PipelineStateDescriptor &desc);
    virtual ~PipelineState() {}

private:
    PipelineStateImpl*      mImpl;
};
} // namespace LimitEngine

#endif // LIMITENGINEV2_PIPELINESTATE_H_