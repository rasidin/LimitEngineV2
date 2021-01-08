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
@file   PrivateDefinitions_DirectX12.h
@brief  Private definitions for DirectX12
@author minseob(leeminseob@outlook.com)
**********************************************************************/
#pragma once
#include <d3d12.h>

#include "Core/Object.h"

namespace LimitEngine{
enum class CommandQueueType
{
    Graphics = 0,
    Compute,
    Copy,
    Num
};
static constexpr uint32 CommandQueueTypeNum = static_cast<uint32>(CommandQueueType::Num);
static constexpr LPCWSTR CommandQueueTypeName[] = {
    L"Graphics",
    L"Compute",
    L"Copy"
};
static constexpr LPCWSTR CommandQueueTypeFenceName[] = {
    L"GraphicsFence",
    L"ComputeFence",
    L"CopyFence",
};
static const D3D12_COMMAND_LIST_TYPE CommandQueueTypeToD3D12[] = {
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY,
};
struct CommandInit_Parameter : public Object<LimitEngineMemoryCategory::Graphics>
{
    ID3D12Device            *mD3DDevice;
    ID3D12CommandQueue      *mCommandQueue[CommandQueueTypeNum];
    ID3D12Fence             *mCommandQueueFence[CommandQueueTypeNum];
};
}