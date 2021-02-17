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
@file  PipelineState.cpp
@brief PipelineState
@author minseob (https://github.com/rasidin)
**********************************************************************/
#include "Renderer/PipelineState.h"

#ifdef USE_DX12
#include "../Platform/DirectX12/PipelineStateImpl_DirectX12.inl"
#else
#error No implementatios of this platform!
#endif

namespace LimitEngine {
PipelineStateRendererAccessor::PipelineStateRendererAccessor(const PipelineState* state)
    : mImpl(state->mImpl)
{}

PipelineState::PipelineState()
    : mImpl(nullptr)
{
#ifdef USE_DX12
    mImpl = new PipelineStateImpl_DirectX12();
#else
#error No implementation of this platform!
#endif
}
} // namespace LimitEngine