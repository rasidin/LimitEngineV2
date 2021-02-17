/*******************************************************************
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
--------------------------------------------------------------------
@file  PipelineStateImpl_DirectX12.inl
@brief PipelineState (DirectX12)
@author minseob (leeminseob@outlook.com)
********************************************************************/
#ifndef LIMITENGINEV2_PIPELINESTATEIMPL_DIRECTX12_INL_
#define LIMITENGINEV2_PIPELINESTATEIMPL_DIRECTX12_INL_

#include <d3d12.h>

#include "Core/Singleton.h"
#include "Core/Memory.h"
#include "Core/MemoryAllocator.h"
#include "Containers/VectorArray.h"
#include "Containers/MapArray.h"
#include "Managers/DrawManager.h"
#include "Renderer/PipelineStateDescriptor.h"
#include "Renderer/Shader.h" 

#include "PrivateDefinitions_DirectX12.h"

namespace LimitEngine {
struct RootSignatureDescriptor 
{
    uint32 ConstantBufferCount[static_cast<int>(Shader::Type::Num)] = { 0u, 0u, 0u };
    uint32 ShaderResourceViewCount[static_cast<int>(Shader::Type::Num)] = { 0u, 0u, 0u };
    uint32 UnorderedAccessViewCount[static_cast<int>(Shader::Type::Num)] = { 0u, 0u, 0u };
    uint32 TextureSamplerCount[static_cast<int>(Shader::Type::Num)] = { 0u, 0u, 0u };

    uint32 NumOfParameters = 0u;

    RootSignatureDescriptor() {}
    RootSignatureDescriptor(Shader** shader)
    {
        if (shader == nullptr) return;
        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (shader[shidx]) {
                ConstantBufferCount[shidx] = shader[shidx]->GetConstantBufferCount();
                if (shader[shidx]->GetConstantBufferCount()) NumOfParameters++;
                ShaderResourceViewCount[shidx] = shader[shidx]->GetBoundTextureCount();
                if (shader[shidx]->GetBoundTextureCount()) NumOfParameters++;
                UnorderedAccessViewCount[shidx] = shader[shidx]->GetUAVCount();
                if (shader[shidx]->GetUAVCount()) NumOfParameters++;
                TextureSamplerCount[shidx] = shader[shidx]->GetBoundSamplerCount();
                if (shader[shidx]->GetBoundSamplerCount()) NumOfParameters++;
            }
        }
    }

    bool operator == (const RootSignatureDescriptor& rsd) const {
        if (NumOfParameters != rsd.NumOfParameters) return false;

        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (ConstantBufferCount[shidx] != rsd.ConstantBufferCount[shidx]) return false;
            if (ShaderResourceViewCount[shidx] != rsd.ShaderResourceViewCount[shidx]) return false;
            if (UnorderedAccessViewCount[shidx] != rsd.UnorderedAccessViewCount[shidx]) return false;
            if (TextureSamplerCount[shidx] != rsd.TextureSamplerCount[shidx]) return false;
        }
        return true;
    }
    bool operator != (const RootSignatureDescriptor& rsd) const {
        if (NumOfParameters != rsd.NumOfParameters) return true;
        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (ConstantBufferCount[shidx] != rsd.ConstantBufferCount[shidx]) return true;
            if (ShaderResourceViewCount[shidx] != rsd.ShaderResourceViewCount[shidx]) return true;
            if (UnorderedAccessViewCount[shidx] != rsd.UnorderedAccessViewCount[shidx]) return true;
            if (TextureSamplerCount[shidx] != rsd.TextureSamplerCount[shidx]) return true;
        }
        return false;
    }
};

class RootSignature
{
public:
    RootSignature()
        : mRootSignature(nullptr)
    {}
    RootSignature(const RootSignatureDescriptor &desc)
        : mRootSignature(nullptr)
    {
        Initialize(desc);
    }
    virtual ~RootSignature()
    {}

    void Initialize(const RootSignatureDescriptor& desc)
    {
        if (ID3D12RootSignature** foundrs = mRootSignatureCache.Find(desc)) {
            mRootSignature = *foundrs;
        }
        else {
            mRootSignature = Create(desc);
        }
    }

    static void ClearAllCaches()
    {
        for (auto rscache : mRootSignatureCache) {
            rscache.value->Release();
        }
        mRootSignatureCache.Clear();
    }

    bool IsValid() const { return mRootSignature != nullptr; }

    inline ID3D12RootSignature* GetRootSignature() const { return mRootSignature; }
private:
    ID3D12RootSignature* Create(const RootSignatureDescriptor& rsd) {
        static constexpr uint32 MaxRootParameterNum = 32u;
        static constexpr D3D12_SHADER_VISIBILITY ShaderTypeToD3D12ShaderVisibility[] = {
            D3D12_SHADER_VISIBILITY_VERTEX,
            D3D12_SHADER_VISIBILITY_PIXEL,
            D3D12_SHADER_VISIBILITY_ALL,
        };

        D3D12_ROOT_PARAMETER rootparams[MaxRootParameterNum];
        D3D12_ROOT_SIGNATURE_DESC rsdesc;
        rsdesc.NumParameters = rsd.NumOfParameters;
        rsdesc.pParameters = &rootparams[0];
        rsdesc.NumStaticSamplers = 0;
        rsdesc.pStaticSamplers = nullptr;
        rsdesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        D3D12_DESCRIPTOR_RANGE descranges[MaxRootParameterNum];
        ::memset(descranges, 0, sizeof(descranges));

        uint32 rootparamindex = 0u;
        uint32 descrangeindex = 0u;
        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (rsd.ConstantBufferCount[shidx]) {
                D3D12_ROOT_PARAMETER& rootparam = rootparams[rootparamindex++];
                rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
                rootparam.ShaderVisibility = ShaderTypeToD3D12ShaderVisibility[shidx];
                rootparam.Descriptor.ShaderRegister = 0;
                rootparam.Descriptor.RegisterSpace = 0;
            }
        }
        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (rsd.TextureSamplerCount[shidx]) {
                D3D12_ROOT_PARAMETER& rootparam = rootparams[rootparamindex++];
                rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootparam.ShaderVisibility = ShaderTypeToD3D12ShaderVisibility[shidx];
                rootparam.DescriptorTable.NumDescriptorRanges = 1;
                D3D12_DESCRIPTOR_RANGE* descrange = &descranges[descrangeindex++];
                rootparam.DescriptorTable.pDescriptorRanges = descrange;

                descrange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                descrange->NumDescriptors = rsd.TextureSamplerCount[shidx];
                descrange->BaseShaderRegister = 0;
                descrange->RegisterSpace = 0;
                descrange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            }
        }
        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (rsd.ShaderResourceViewCount[shidx]) {
                D3D12_ROOT_PARAMETER& rootparam = rootparams[rootparamindex++];
                rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootparam.ShaderVisibility = ShaderTypeToD3D12ShaderVisibility[shidx];
                rootparam.DescriptorTable.NumDescriptorRanges = 1;
                D3D12_DESCRIPTOR_RANGE* descrange = &descranges[descrangeindex++];
                rootparam.DescriptorTable.pDescriptorRanges = descrange;

                descrange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                descrange->NumDescriptors = rsd.ShaderResourceViewCount[shidx];
                descrange->BaseShaderRegister = 0;
                descrange->RegisterSpace = 0;
                descrange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            }
        }
        for (uint32 shidx = 0; shidx < static_cast<uint32>(Shader::Type::Num); shidx++) {
            if (rsd.UnorderedAccessViewCount[shidx]) {
                D3D12_ROOT_PARAMETER& rootparam = rootparams[rootparamindex++];
                rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootparam.ShaderVisibility = ShaderTypeToD3D12ShaderVisibility[shidx];
                rootparam.DescriptorTable.NumDescriptorRanges = 1;
                D3D12_DESCRIPTOR_RANGE* descrange = &descranges[descrangeindex++];
                rootparam.DescriptorTable.pDescriptorRanges = &descranges[descrangeindex++];

                descrange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                descrange->NumDescriptors = rsd.UnorderedAccessViewCount[shidx];
                descrange->BaseShaderRegister = 0;
                descrange->RegisterSpace = 0;
                descrange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            }
        }

        ID3D12RootSignature* output{ nullptr };
        if (ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle()) {
            ID3DBlob* outblob;
            ID3DBlob* errorblob;
            if (SUCCEEDED(D3D12SerializeRootSignature(&rsdesc, D3D_ROOT_SIGNATURE_VERSION_1, &outblob, &errorblob))) {
                if (SUCCEEDED(device->CreateRootSignature(1, outblob->GetBufferPointer(), outblob->GetBufferSize(), IID_PPV_ARGS(&output)))) {
                    mRootSignatureCache.Add(rsd, output);
                }
                else {
                    LEASSERT(false);
                }
                if (outblob)
                    outblob->Release();
                if (errorblob)
                    errorblob->Release();
            }
            else {
                DEBUG_MESSAGE((char*)errorblob->GetBufferPointer());
                LEASSERT(false);
            }
        }

        return output;
    }

private:
    static MapArray<RootSignatureDescriptor, ID3D12RootSignature*> mRootSignatureCache;
    ID3D12RootSignature* mRootSignature;
};
MapArray<RootSignatureDescriptor, ID3D12RootSignature*> RootSignature::mRootSignatureCache;

class PipelineStateObject
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC ConvertToGraphicsPipelineStateDesc(const RootSignature& rootsignature, const PipelineStateDescriptor& desc)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC output;
        ::memset(&output, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

        output.NodeMask = 1;
        output.SampleMask = 0xffffffffu;
        output.SampleDesc.Count = 1;

        output.pRootSignature = rootsignature.GetRootSignature();

        if (Shader* vertexshader = desc.Shaders[static_cast<int>(Shader::Type::Vertex)].Get()) {
            output.VS.pShaderBytecode = vertexshader->GetCompiledCodeBin();
            output.VS.BytecodeLength  = vertexshader->GetCompiledCodeSize();
        }
        if (Shader* pixelshader = desc.Shaders[static_cast<int>(Shader::Type::Pixel)].Get()) {
            output.PS.pShaderBytecode = pixelshader->GetCompiledCodeBin();
            output.PS.BytecodeLength  = pixelshader->GetCompiledCodeSize();
        }

        output.RasterizerState.CullMode = CullModeToD3D12CullMode[static_cast<int>(desc.RasterizerDescriptor.CullMode)];
        output.RasterizerState.FillMode = FillModeToD3D12FillMode[static_cast<int>(desc.RasterizerDescriptor.FillMode)];
        output.RasterizerState.FrontCounterClockwise = desc.RasterizerDescriptor.Culling == RendererFlag::Culling::CounterClockWise;

        output.InputLayout.NumElements = desc.InputElementCount;
        if (desc.InputElementCount) {
            InputElementDescs = (D3D12_INPUT_ELEMENT_DESC*)MemoryAllocator::Alloc(desc.InputElementCount * sizeof(D3D12_INPUT_ELEMENT_DESC));
            for (uint32 ieidx = 0; ieidx < desc.InputElementCount; ieidx++) {
                D3D12_INPUT_ELEMENT_DESC& currentied = InputElementDescs[ieidx];
                currentied.SemanticName = desc.InputElementDescriptors[ieidx].SemanticName;
                currentied.SemanticIndex = desc.InputElementDescriptors[ieidx].SemanticIndex;
                currentied.Format = BufferFormatToDXGIFormat[static_cast<int>(desc.InputElementDescriptors[ieidx].Format)];
                currentied.InputSlot = desc.InputElementDescriptors[ieidx].InputSlot;
                currentied.AlignedByteOffset = desc.InputElementDescriptors[ieidx].AlignedByteOffset;
                currentied.InputSlotClass = InputClassificationToD3D12InputClassification[static_cast<int>(desc.InputElementDescriptors[ieidx].InputSlotClass)];
                currentied.InstanceDataStepRate = desc.InputElementDescriptors[ieidx].InstanceDataStepRate;
            }
        }
        output.InputLayout.pInputElementDescs = InputElementDescs;

        output.NumRenderTargets = desc.RenderTargetsNum;
        for (uint32 rtvidx = 0; rtvidx < 8; rtvidx++) {
            output.RTVFormats[rtvidx] = BufferFormatToDXGIFormat[static_cast<int>(desc.RenderTargetFormats[rtvidx])];
        }
        output.DSVFormat = BufferFormatToDXGIFormat[static_cast<int>(desc.DepthTargetFormat)];

        output.BlendState.AlphaToCoverageEnable = desc.BlendDescriptor.AlphaToCoverageEnabled;
        output.BlendState.IndependentBlendEnable = desc.BlendDescriptor.IndependentBlendEnabled;
        for (uint32 rtidx = 0; rtidx < 8; rtidx++) {
            output.BlendState.RenderTarget[rtidx].BlendEnable = desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].BlendEnabled;
            output.BlendState.RenderTarget[rtidx].SrcBlend = BlendValuesToD3D12Blend[static_cast<int>(desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].SrcBlend)];
            output.BlendState.RenderTarget[rtidx].DestBlend = BlendValuesToD3D12Blend[static_cast<int>(desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].DestBlend)];
            output.BlendState.RenderTarget[rtidx].BlendOp = BlendOperatorsToD3D12BlendOp[static_cast<int>(desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].BlendOp)];
            output.BlendState.RenderTarget[rtidx].SrcBlendAlpha = BlendValuesToD3D12Blend[static_cast<int>(desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].SrcAlpha)];
            output.BlendState.RenderTarget[rtidx].DestBlendAlpha = BlendValuesToD3D12Blend[static_cast<int>(desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].DestAlpha)];
            output.BlendState.RenderTarget[rtidx].BlendOpAlpha = BlendOperatorsToD3D12BlendOp[static_cast<int>(desc.BlendDescriptor.RenderTargetBlendDescriptor[rtidx].BlendOpAlpha)];
            output.BlendState.RenderTarget[rtidx].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        output.PrimitiveTopologyType = PrimitiveTopologyTypeToD3D12PrimitiveTopologyType[static_cast<int>(desc.Topology)];

        output.DepthStencilState.DepthEnable = desc.DepthStencilDescriptor.DepthEnabled;
        output.DepthStencilState.DepthWriteMask = DepthWriteMaskToD3D12DepthWriteMask[static_cast<int>(desc.DepthStencilDescriptor.DepthWriteMask)];
        output.DepthStencilState.DepthFunc = TestFlagsToD3D12ComparisonFunc[static_cast<int>(desc.DepthStencilDescriptor.DepthFunc)];
        output.DepthStencilState.StencilEnable = desc.DepthStencilDescriptor.StencilEnabled;
        output.DepthStencilState.StencilReadMask = desc.DepthStencilDescriptor.StencilReadMask;
        output.DepthStencilState.StencilWriteMask = desc.DepthStencilDescriptor.StencilWriteMask;
        output.DepthStencilState.FrontFace.StencilFailOp = StencilOperatorsToD3D12StencilOp[static_cast<int>(desc.DepthStencilDescriptor.StencilFrontFace.FailOperator)];
        output.DepthStencilState.FrontFace.StencilDepthFailOp = StencilOperatorsToD3D12StencilOp[static_cast<int>(desc.DepthStencilDescriptor.StencilFrontFace.DepthFailOperator)];
        output.DepthStencilState.FrontFace.StencilPassOp = StencilOperatorsToD3D12StencilOp[static_cast<int>(desc.DepthStencilDescriptor.StencilFrontFace.PassOperator)];
        output.DepthStencilState.FrontFace.StencilFunc = TestFlagsToD3D12ComparisonFunc[static_cast<int>(desc.DepthStencilDescriptor.StencilFrontFace.StencilFunc)];
        output.DepthStencilState.BackFace.StencilFailOp = StencilOperatorsToD3D12StencilOp[static_cast<int>(desc.DepthStencilDescriptor.StencilBackFace.FailOperator)];
        output.DepthStencilState.BackFace.StencilDepthFailOp = StencilOperatorsToD3D12StencilOp[static_cast<int>(desc.DepthStencilDescriptor.StencilBackFace.DepthFailOperator)];
        output.DepthStencilState.BackFace.StencilPassOp = StencilOperatorsToD3D12StencilOp[static_cast<int>(desc.DepthStencilDescriptor.StencilBackFace.PassOperator)];
        output.DepthStencilState.BackFace.StencilFunc = TestFlagsToD3D12ComparisonFunc[static_cast<int>(desc.DepthStencilDescriptor.StencilBackFace.StencilFunc)];
    
        return output;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC ConvertToComputePipelineStateDesc(const RootSignature& rootsignature, const PipelineStateDescriptor& desc)
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC output;
        ::memset(&output, 0, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));

        output.pRootSignature = rootsignature.GetRootSignature();
        if (Shader* computeshader = desc.Shaders[static_cast<int>(Shader::Type::Compute)].Get()) {
            output.CS.pShaderBytecode = computeshader->GetCompiledCodeBin();
            output.CS.BytecodeLength  = computeshader->GetCompiledCodeSize();
        }
        output.NodeMask = 1u;

        return output;
    }

public:
    PipelineStateObject() {}
    PipelineStateObject(const RootSignature& rootsignature, const PipelineStateDescriptor& desc)
    {
        Initialize(rootsignature, desc);
    }
    virtual ~PipelineStateObject()
    {
        if (InputElementDescs) {
            MemoryAllocator::Free(InputElementDescs);
            InputElementDescs = nullptr;
        }
    }
    void Initialize(const RootSignature& rootsignature, const PipelineStateDescriptor& desc)
    {
        ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
        if (!device) return;

        if (ID3D12PipelineState** foundPSO = mPipelineStateCache.Find(desc)) {
            mPipelineStateObject = *foundPSO;
        }
        else {
            ID3D12PipelineState* newpso = nullptr;
            if (desc.Shaders[static_cast<int>(Shader::Type::Pixel)]) {
                D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = ConvertToGraphicsPipelineStateDesc(rootsignature, desc);
                if (FAILED(device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&newpso)))) {
                    LEASSERT(0);
                }
            }
            else if (desc.Shaders[static_cast<int>(Shader::Type::Compute)]) {
                D3D12_COMPUTE_PIPELINE_STATE_DESC psodesc = ConvertToComputePipelineStateDesc(rootsignature, desc);
                if (FAILED(device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(&newpso)))) {
                    LEASSERT(0);
                }
            }
            mPipelineStateCache[desc] = newpso;
            mPipelineStateObject = newpso;
        }
    }
    bool IsValid() const { return mPipelineStateObject != nullptr; }
    ID3D12PipelineState* GetObject() const { return mPipelineStateObject; }
private:
    static MapArray<PipelineStateDescriptor, ID3D12PipelineState*> mPipelineStateCache;
    ID3D12PipelineState* mPipelineStateObject = nullptr;
    D3D12_INPUT_ELEMENT_DESC* InputElementDescs = nullptr;
};
MapArray<PipelineStateDescriptor, ID3D12PipelineState*> PipelineStateObject::mPipelineStateCache;

class PipelineStateImpl_DirectX12 : public PipelineStateImpl
{
public:
    PipelineStateImpl_DirectX12() {}
    virtual ~PipelineStateImpl_DirectX12() {}
    virtual bool IsValid() const override { return mRootSignature.IsValid() && mPipelineStateObject.IsValid(); }

    virtual bool Init(const PipelineStateDescriptor& desc) override
    {
        Shader* shaders[static_cast<int>(Shader::Type::Num)] = { nullptr, nullptr, nullptr };
        if (desc.Shaders[static_cast<int>(Shader::Type::Vertex)]) {
            shaders[static_cast<int>(Shader::Type::Vertex)] = desc.Shaders[static_cast<int>(Shader::Type::Vertex)].Get();
            shaders[static_cast<int>(Shader::Type::Pixel)] = desc.Shaders[static_cast<int>(Shader::Type::Pixel)].Get();
        }
        else if (desc.Shaders[static_cast<int>(Shader::Type::Compute)]) {
            shaders[static_cast<int>(Shader::Type::Compute)] = desc.Shaders[static_cast<int>(Shader::Type::Compute)].Get();
        }
        mRootSignature.Initialize(RootSignatureDescriptor(shaders));
        mPipelineStateObject.Initialize(mRootSignature, desc);

        return true;
    }

    virtual void* GetRootSignatureHandle() const override { return mRootSignature.GetRootSignature(); }
    virtual void* GetHandle() const override { return mPipelineStateObject.GetObject(); }

private:
    RootSignature       mRootSignature;
    PipelineStateObject mPipelineStateObject;
};
}

#endif // LIMITENGINEV2_PIPELINESTATEIMPL_DIRECTX12_INL_