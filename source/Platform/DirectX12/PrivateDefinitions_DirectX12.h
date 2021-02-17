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
@author minseob(https://github.com/rasidin)
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
    L"CopyFence"
};
static const D3D12_COMMAND_LIST_TYPE CommandQueueTypeToD3D12[] = {
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY
};
static constexpr D3D12_BLEND BlendValuesToD3D12Blend[] = {
    D3D12_BLEND_ZERO,               // Zero
    D3D12_BLEND_ONE,                // One
    D3D12_BLEND_SRC_COLOR,          // SrcColor
    D3D12_BLEND_INV_SRC_COLOR,      // InvSrcColor
    D3D12_BLEND_SRC_ALPHA,          // SrcAlpha
    D3D12_BLEND_INV_SRC_ALPHA,      // InvSrcAlpha
    D3D12_BLEND_DEST_ALPHA,         // DestAlpha
    D3D12_BLEND_INV_DEST_ALPHA,     // InvDestAlpha
    D3D12_BLEND_DEST_COLOR,         // DestColor
    D3D12_BLEND_INV_DEST_COLOR      // InvDestColor
};
static constexpr D3D12_BLEND_OP BlendOperatorsToD3D12BlendOp[] = {
    D3D12_BLEND_OP_ADD,             // Add
    D3D12_BLEND_OP_SUBTRACT,        // Subtract
    D3D12_BLEND_OP_REV_SUBTRACT,    // RevSubtract
    D3D12_BLEND_OP_MIN,             // Min
    D3D12_BLEND_OP_MAX              // Max
};
static constexpr DXGI_FORMAT BufferFormatToDXGIFormat[] = {
    DXGI_FORMAT_UNKNOWN,
    DXGI_FORMAT_R32G32B32A32_TYPELESS,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_R32G32B32_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32_UINT,
    DXGI_FORMAT_R32G32B32_SINT,
    DXGI_FORMAT_R16G16B16A16_TYPELESS,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_UNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_R32G32_TYPELESS,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32_UINT,
    DXGI_FORMAT_R32G32_SINT,
    DXGI_FORMAT_R10G10B10A2_TYPELESS,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_UINT,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R8G8B8A8_SINT,
    DXGI_FORMAT_R16G16_TYPELESS,
    DXGI_FORMAT_R16G16_FLOAT,
    DXGI_FORMAT_R16G16_UNORM,
    DXGI_FORMAT_R16G16_UINT,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_R16G16_SINT,
    DXGI_FORMAT_R32_FLOAT,
    DXGI_FORMAT_D32_FLOAT,
    DXGI_FORMAT_R32_UINT,
    DXGI_FORMAT_R32_SINT,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_R16_TYPELESS,
    DXGI_FORMAT_R16_FLOAT,
    DXGI_FORMAT_D16_UNORM,
    DXGI_FORMAT_R16_UNORM,
    DXGI_FORMAT_R16_SINT,
    DXGI_FORMAT_R8_TYPELESS,
    DXGI_FORMAT_R8_UNORM,
    DXGI_FORMAT_R8_UINT,
    DXGI_FORMAT_R8_SNORM,
    DXGI_FORMAT_R8_SINT
};
inline DXGI_FORMAT ConvertBufferFormatToDXGIFormat(const RendererFlag::BufferFormat& format) { return BufferFormatToDXGIFormat[static_cast<int>(format)]; }
static constexpr D3D12_INPUT_CLASSIFICATION InputClassificationToD3D12InputClassification[] = {
    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,     // PerVertex
    D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA    // PerInstanceData
};
static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyTypeToD3D12PrimitiveTopologyType[] = {
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,    // Undefined
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,        // Point
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,         // Line
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,     // Triangle
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH         // Patch
};
static constexpr D3D_PRIMITIVE_TOPOLOGY PrimitiveTopologyTypeToD3DPrimitiveTopology[] = {
    D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,           // Undefined
    D3D_PRIMITIVE_TOPOLOGY_POINTLIST,           // Point
    D3D_PRIMITIVE_TOPOLOGY_LINELIST,            // Line
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,        // Triangle
    D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,           // Undefined
};
static constexpr D3D12_DEPTH_WRITE_MASK DepthWriteMaskToD3D12DepthWriteMask[] = {
    D3D12_DEPTH_WRITE_MASK_ZERO,
    D3D12_DEPTH_WRITE_MASK_ALL
};
static constexpr D3D12_COMPARISON_FUNC TestFlagsToD3D12ComparisonFunc[] = {
    D3D12_COMPARISON_FUNC_NEVER,
    D3D12_COMPARISON_FUNC_LESS,
    D3D12_COMPARISON_FUNC_EQUAL,
    D3D12_COMPARISON_FUNC_LESS_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER,
    D3D12_COMPARISON_FUNC_NOT_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER_EQUAL,
    D3D12_COMPARISON_FUNC_ALWAYS
};
static constexpr D3D12_STENCIL_OP StencilOperatorsToD3D12StencilOp[] = {
    D3D12_STENCIL_OP_KEEP,
    D3D12_STENCIL_OP_ZERO,
    D3D12_STENCIL_OP_REPLACE,
    D3D12_STENCIL_OP_INCR_SAT,
    D3D12_STENCIL_OP_DECR_SAT,
    D3D12_STENCIL_OP_INVERT,
    D3D12_STENCIL_OP_INCR,
    D3D12_STENCIL_OP_DECR
};
static constexpr D3D12_FILL_MODE FillModeToD3D12FillMode[] = {
    D3D12_FILL_MODE_WIREFRAME,
    D3D12_FILL_MODE_SOLID
};
static constexpr D3D12_CULL_MODE CullModeToD3D12CullMode[] = {
    D3D12_CULL_MODE_NONE,
    D3D12_CULL_MODE_FRONT,
    D3D12_CULL_MODE_BACK
};
struct CommandInit_Parameter : public Object<LimitEngineMemoryCategory::Graphics>
{
    ID3D12Device            *mD3DDevice;
    ID3D12CommandQueue      *mCommandQueue[CommandQueueTypeNum];
    ID3D12Fence             *mCommandQueueFence[CommandQueueTypeNum];
};
}