/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  CommandImpl_DirectX11.inl
@brief CommandBuffer for rendering
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include <d3d11.h>

#include <LERenderer>

#include "Renderer/CommandBuffer.h"
#include "Renderer/Texture.h"
#include "Managers/DrawManager.h"

#include "PrivateDefinitions_DirectX11.h"

namespace LimitEngine {
    // Utilites
    D3D11_PRIMITIVE_TOPOLOGY ConvertPrimitiveType(uint32 type)
    {
        switch (static_cast<RendererFlag::PrimitiveTypes>(type))
        {
        case RendererFlag::PrimitiveTypes::POINTLIST:       return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case RendererFlag::PrimitiveTypes::LINELIST:        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case RendererFlag::PrimitiveTypes::LINESTRIP:       return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case RendererFlag::PrimitiveTypes::TRIANGLELIST:    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case RendererFlag::PrimitiveTypes::TRIANGLESTRIP:   return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:                                            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        }
    }
    class CommandImpl_DirectX11 : public CommandImpl
    {
        static constexpr uint32 RenderTargetMaxCount = 8;

        enum DepthState {
            DepthStateTest = 1,
            DepthStateWrite = 1 << 1,
            DepthStateStencil = 1 << 2,
            DepthStateCount = 1 << 3,

            DepthStateDefault = DepthStateTest | DepthStateWrite,
        };
        enum BlendState {
            BlendStateTest = 1,
            BlendStateBlend = 1 << 1,
            BlendStateCount = 1 << 2,

            BlendStateDefault = 0,
        };
        struct BlendStateID
        {
            uint32 enabledFlag;
            D3D11_BLEND sourceColor[RenderTargetMaxCount];
            D3D11_BLEND destColor[RenderTargetMaxCount];
            D3D11_BLEND sourceAlpha[RenderTargetMaxCount];
            D3D11_BLEND destAlpha[RenderTargetMaxCount];
            D3D11_BLEND_OP blendColor[RenderTargetMaxCount];
            D3D11_BLEND_OP blendAlpha[RenderTargetMaxCount];

            BlendStateID() : enabledFlag(0) {}
            BlendStateID(uint32 enable, D3D11_BLEND *src, D3D11_BLEND *dst, D3D11_BLEND_OP *srcBlend, D3D11_BLEND_OP *dstBlend)
            {
                enabledFlag = enable;
                ::memcpy(sourceColor, src, sizeof(sourceColor));
                ::memcpy(destColor, dst, sizeof(destColor));
                ::memcpy(sourceAlpha, srcBlend, sizeof(sourceAlpha));
                ::memcpy(destAlpha, dstBlend, sizeof(destAlpha));
                ::memcpy(blendColor, srcBlend, sizeof(blendColor));
                ::memcpy(blendAlpha, dstBlend, sizeof(blendAlpha));
            }
            bool operator==(const BlendStateID &id) const
            {
                if (enabledFlag != id.enabledFlag)
                    return false;
                for (uint32 boidx = 0; boidx < RenderTargetMaxCount; boidx++) {
                    if (sourceColor[boidx] != id.sourceColor[boidx] || destColor[boidx] != id.destColor[boidx] ||
                        sourceAlpha[boidx] != id.sourceAlpha[boidx] || destAlpha[boidx] != id.destAlpha[boidx] ||
                        blendColor[boidx] != id.blendColor[boidx] || blendAlpha[boidx] != id.blendAlpha[boidx])
                        return false;
                }
                return true;
            }
        };
        typedef uint32 DepthStateID;
        struct Cache
        {
            Shader                  *CurrentShader;
            uint32                   CurrentFVF;
            uint32                   DepthState;
            RendererFlag::TestFlags  DepthFunc;
            bool                     DepthStateModified;
            BlendStateID             BlendState;
            bool                     BlendStateModified;
            uint32                   Culling;
            bool                     CullingModified;
            Cache()
                : CurrentShader(nullptr)
                , CurrentFVF(0u)
                , DepthState(DepthStateDefault)
                , DepthFunc(RendererFlag::TestFlags::LESS)
                , DepthStateModified(true)
                , BlendState()
                , BlendStateModified(true)
                , Culling(0u)
                , CullingModified(false)
            {}
        } cache;

        MapArray<DepthStateID, ID3D11DepthStencilState*> mDepthStencilStates;
        MapArray<BlendStateID, ID3D11BlendState*>        mBlendStates;
    public:
        CommandImpl_DirectX11()
            : CommandImpl()
            , cache() 
            , mD3DDevice(nullptr)
            , mD3DDeviceContext(nullptr)
            , mDefaultSampler(nullptr)
            , mBaseRenderTargetView(nullptr)
            , mBaseDepthStencilView(nullptr)
        {}
        virtual ~CommandImpl_DirectX11() {}

        void Init(void *Parameter) override
        {
            if (!Parameter) return;

            CommandInit_Parameter *InitParameter = (CommandInit_Parameter*)Parameter;
            mD3DDevice = InitParameter->mD3DDevice;
            mD3DDeviceContext = InitParameter->mD3DDeviceContext;
            mBaseRenderTargetView = InitParameter->mBaseRenderTargetView;
            mBaseDepthStencilView = InitParameter->mBaseDepthStencilView;

            // Set BlendState
            for (uint32 bfidx = 0; bfidx < RenderTargetMaxCount; bfidx++)
                SetBlendFunc(bfidx, RendererFlag::BlendFlags::SOURCE);
            // Set DepthState
            SetDepthFunc(static_cast<uint32>(RendererFlag::TestFlags::LESS));

            if (mD3DDevice) {
                // Make rasterizer
                for (uint32 rastIdx = 0; rastIdx < static_cast<uint32>(RendererFlag::Culling::Max); rastIdx++) {
                    D3D11_RASTERIZER_DESC rastDesc;
                    ::memset(&rastDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
                    rastDesc.FillMode = D3D11_FILL_SOLID;
                    rastDesc.DepthClipEnable = true;
                    switch (static_cast<RendererFlag::Culling>(rastIdx)) {
                    case RendererFlag::Culling::ClockWise: {
                        rastDesc.CullMode = D3D11_CULL_BACK;
                        rastDesc.FrontCounterClockwise = false;
                    } break;
                    case RendererFlag::Culling::CounterClockWise: {
                        rastDesc.CullMode = D3D11_CULL_BACK;
                        rastDesc.FrontCounterClockwise = true;
                    } break;
                    case RendererFlag::Culling::None: {
                        rastDesc.CullMode = D3D11_CULL_NONE;
                    } break;
                    }
                    mD3DDevice->CreateRasterizerState(&rastDesc, &mRasterizerStates[rastIdx]);
                }
                // Create default sampler
                D3D11_SAMPLER_DESC samplerDesc;
                ::memset(&samplerDesc, 0, sizeof(samplerDesc));
                samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
                samplerDesc.MinLOD = 0;
                samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
                mD3DDevice->CreateSamplerState(&samplerDesc, &mDefaultSampler);
            }
        }
        void Term() override
        {
            for (uint32 blendIndex = 0; blendIndex < mBlendStates.GetSize(); blendIndex++) {
                if (mBlendStates.GetAt(blendIndex).value) {
                    mBlendStates.GetAt(blendIndex).value->Release();
                    mBlendStates.GetAt(blendIndex).value = NULL;
                }
            }
            mBlendStates.Clear();
            for (uint32 depthIndex = 0; depthIndex < mDepthStencilStates.GetSize(); depthIndex++) {
                mDepthStencilStates.GetAt(depthIndex).value->Release();
            }
            mDepthStencilStates.Clear();
            if (mDefaultSampler) {
                mDefaultSampler->Release();
            }
            mDefaultSampler = nullptr;
        }

        void BeginScene() override {}
        void EndScene() override {}
        bool PrepareForDrawing() override
        {
            LEASSERT(mD3DDeviceContext);

            // Set Rasterize
            if (cache.CullingModified) {
                mD3DDeviceContext->RSSetState(mRasterizerStates[cache.Culling]);
                cache.CullingModified = false;
            }
            // Set DepthState
            if (cache.DepthStateModified)
            {
                if (ID3D11DepthStencilState *dss = getDepthStencilState(cache.DepthState, cache.DepthFunc)) {
                    mD3DDeviceContext->OMSetDepthStencilState(dss, 0);
                }
                cache.DepthStateModified = false;
            }
            // Set BlendState
            if (cache.BlendStateModified)
            {
                if (ID3D11BlendState *bs = getBlendState(cache.BlendState)) {
                    float blendFactor[4] = { 0,0,0,0 };
                    mD3DDeviceContext->OMSetBlendState(bs, blendFactor, 0xffffffff);
                }
                cache.BlendStateModified = false;
            }
            // Set FVF
            if (cache.CurrentShader)
            {
                if (ID3D11InputLayout *inputLayout = (ID3D11InputLayout*)cache.CurrentShader->GetInputLayout(mD3DDevice, cache.CurrentFVF)) {
                    mD3DDeviceContext->IASetInputLayout(inputLayout);
                }
                else
                    return false;
            }
            // Ready shader before drawing
            if (cache.CurrentShader) {
                cache.CurrentShader->PrepareForDrawing();
            }
            return true;
        }
        bool PrepareForDrawingModel() override { return true; }
        void ClearScreen(const LEMath::FloatColorRGBA &color) override
        {
            if (mBaseRenderTargetView) {
                FLOAT color4[4] = { color.Red(), color.Green(), color.Blue(), color.Alpha() };
                mD3DDeviceContext->ClearRenderTargetView(mBaseRenderTargetView, color4);
            }
            if (mBaseDepthStencilView) {
                mD3DDeviceContext->ClearDepthStencilView(mBaseDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            }
        }
        void BindVertexBuffer(void *handle, void *buffer, uint32 offset, uint32 size, uint32 stride) override
        {
            if (!handle) return;

            if (mD3DDeviceContext) {
                ID3D11Buffer *vbHandle = (ID3D11Buffer *)handle;
                if (buffer) {
                    D3D11_MAPPED_SUBRESOURCE mapSR;
                    if (SUCCEEDED(mD3DDeviceContext->Map(vbHandle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSR))) {
                        memcpy(mapSR.pData, buffer, size);
                        mD3DDeviceContext->Unmap(vbHandle, 0);
                    }
                }
                ID3D11Buffer* pBufferTbl[] = { vbHandle };
                UINT SizeTbl[] = { stride };
                UINT OffsetTbl[] = { offset };
                mD3DDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);
            }
        }
        void BindIndexBuffer(void *handle) override
        {
            if (!handle) return;

            if (mD3DDeviceContext) {
                ID3D11Buffer *idxHandle = (ID3D11Buffer *)handle;
                mD3DDeviceContext->IASetIndexBuffer(idxHandle, DXGI_FORMAT_R32_UINT, 0);
            }
        }
        void BindShader(Shader *Shader) override
        {
            if (Shader)
                Shader->Bind();
            cache.CurrentShader = Shader;
        }
        void BindTargetTexture(uint32 index, Texture *texture) override
        {
            if (cache.CurrentShader) {
                if (cache.CurrentShader->HasShader(Shader::TYPE_COMPUTE)) {
                    if (texture == NULL) {
                        ID3D11UnorderedAccessView* uavs[] = { NULL };
                        mD3DDeviceContext->CSSetUnorderedAccessViews(index, 1, uavs, NULL);
                    }
                    else {
                        ID3D11UnorderedAccessView* uavs[] = { (ID3D11UnorderedAccessView*)texture->GetUnorderedAccessView() };
                        mD3DDeviceContext->CSSetUnorderedAccessViews(index, 1, uavs, (UINT*)(&uavs[0]));
                    }
                }
            }
        }
        void BindTexture(uint32 Index, Texture *Texture) override
        {
            LEASSERT(mD3DDeviceContext);
            if (cache.CurrentShader) {
                if (cache.CurrentShader->HasShader(Shader::TYPE_PIXEL)) {
                    ID3D11SamplerState* sss[] = { mDefaultSampler };
                    mD3DDeviceContext->PSSetSamplers(Index, 1, sss);
                    ID3D11ShaderResourceView* srvs[] = { (ID3D11ShaderResourceView*)Texture->GetShaderResourceView() };
                    mD3DDeviceContext->PSSetShaderResources(Index, 1, srvs);
                }
                else if (cache.CurrentShader->HasShader(Shader::TYPE_COMPUTE)) {
                    ID3D11SamplerState* sss[] = { mDefaultSampler };
                    mD3DDeviceContext->CSSetSamplers(Index, 1, sss);
                    ID3D11ShaderResourceView* srvs[] = { (ID3D11ShaderResourceView*)Texture->GetShaderResourceView() };
                    mD3DDeviceContext->CSSetShaderResources(Index, 1, srvs);
                }
            }
        }
        void Dispatch(int x, int y, int z) override
        {
            if (mD3DDeviceContext) {
                mD3DDeviceContext->Dispatch(x, y, z);
            }
        }
        void DrawPrimitive(uint32 type, uint32 offset, uint32 count) override
        {
            if (mD3DDeviceContext) {
                mD3DDeviceContext->IASetPrimitiveTopology(ConvertPrimitiveType(type));
                mD3DDeviceContext->Draw(count, offset);
                mD3DDeviceContext->Flush();
            }
        }
        void DrawPrimitiveUp(uint32 type, uint32 count, void *data, uint32 stride) override
        { // Unimplemented
            if (mD3DDeviceContext) {
                mD3DDeviceContext->IASetPrimitiveTopology(ConvertPrimitiveType(type));
            }
        }
        void DrawIndexedPrimitive(uint32 type, uint32 vtxcount, uint32 count) override
        {
            if (mD3DDeviceContext) {
                mD3DDeviceContext->IASetPrimitiveTopology(ConvertPrimitiveType(type));
                mD3DDeviceContext->DrawIndexed(count, 0, 0);
                mD3DDeviceContext->Flush();
            }
        }
        void SetFVF(uint32 fvf) override { cache.CurrentFVF = fvf; }
        void SetCulling(uint32 cull) override
        {
            if (cache.Culling != cull) {
                cache.Culling = cull;
                cache.CullingModified = true;
            }
        }
        void SetBlendFunc(uint32 rt, RendererFlag::BlendFlags Func) override
        {
            switch (Func) {
            case RendererFlag::BlendFlags::SOURCE:
                cache.BlendState.sourceColor[rt] = D3D11_BLEND_ONE;
                cache.BlendState.destColor[rt] = D3D11_BLEND_ZERO;
                cache.BlendState.blendColor[rt] = D3D11_BLEND_OP_ADD;
                cache.BlendState.sourceAlpha[rt] = D3D11_BLEND_ONE;
                cache.BlendState.destAlpha[rt] = D3D11_BLEND_ZERO;
                cache.BlendState.blendAlpha[rt] = D3D11_BLEND_OP_ADD;
                break;
            case RendererFlag::BlendFlags::ALPHABLEND:
                cache.BlendState.sourceColor[rt] = D3D11_BLEND_SRC_ALPHA;
                cache.BlendState.destColor[rt] = D3D11_BLEND_INV_SRC_ALPHA;
                cache.BlendState.blendColor[rt] = D3D11_BLEND_OP_ADD;
                cache.BlendState.sourceAlpha[rt] = D3D11_BLEND_ONE;
                cache.BlendState.destAlpha[rt] = D3D11_BLEND_ZERO;
                cache.BlendState.blendAlpha[rt] = D3D11_BLEND_OP_ADD;
                break;
            case RendererFlag::BlendFlags::ADD:
                cache.BlendState.sourceColor[rt] = D3D11_BLEND_SRC_ALPHA;
                cache.BlendState.destColor[rt] = D3D11_BLEND_ONE;
                cache.BlendState.blendColor[rt] = D3D11_BLEND_OP_ADD;
                cache.BlendState.sourceAlpha[rt] = D3D11_BLEND_ONE;
                cache.BlendState.destAlpha[rt] = D3D11_BLEND_ZERO;
                cache.BlendState.blendAlpha[rt] = D3D11_BLEND_OP_ADD;
                break;
            default: break;
            }
            cache.BlendStateModified = true;
        }
        void SetDepthFunc(uint32 Func) override
        {
            cache.DepthFunc = static_cast<RendererFlag::TestFlags>(Func);
            cache.DepthStateModified = true;
        }
        void SetEnabled(uint32 flag) override
        {
            if (flag & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_TEST)
                && (cache.DepthState & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_TEST)) == 0) {
                cache.DepthState |= DepthStateTest;
                cache.DepthStateModified = true;
            }
            if (flag & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_WRITE)
                && (cache.DepthState & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_WRITE)) == 0) {
                cache.DepthState |= DepthStateWrite;
                cache.DepthStateModified = true;
            }

            if (flag & static_cast<uint32>(RendererFlag::EnabledFlags::ALPHABLEND)) {
                cache.BlendState.enabledFlag |= 1 << ((flag >> 8) & 0xff);
                cache.BlendStateModified = true;
            }
        }
        void SetDisable(uint32 flag) override
        {
            if (flag & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_TEST)
                && (cache.DepthState & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_TEST))) {
                cache.DepthState &= ~DepthStateTest;
                cache.DepthStateModified = true;
            }
            if (flag & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_WRITE)
                && (cache.DepthState & static_cast<uint32>(RendererFlag::EnabledFlags::DEPTH_WRITE))) {
                cache.DepthState &= ~DepthStateWrite;
                cache.DepthStateModified = true;
            }

            if (flag & static_cast<uint32>(RendererFlag::EnabledFlags::ALPHABLEND)) {
                cache.BlendState.enabledFlag &= ~(1 << ((flag >> 8) & 0xff));
                cache.BlendStateModified = true;
            }
        }
        void SetRenderTarget(uint32 index, Texture *color, Texture *depth, uint32 surfaceIndex) override {
            if (mD3DDevice) {
                ID3D11RenderTargetView *renderTargetView = color ? ((ID3D11RenderTargetView*)color->GetRenderTargetView()) : ((ID3D11RenderTargetView*)LE_DrawManager.GetFrameBuffer());
                ID3D11DepthStencilView *depthStencilView = depth ? ((ID3D11DepthStencilView*)depth->GetDepthStencilView()) : nullptr;

                mD3DDeviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
            }
        }
    private:
        inline DepthStateID makeDepthStateIndex(uint32 flag, RendererFlag::TestFlags funcFlag) { return uint32((flag << 8) | static_cast<uint32>(funcFlag)); }
        ID3D11DepthStencilState* getDepthStencilState(uint32 flag, RendererFlag::TestFlags funcFlag)
        {
            const static D3D11_COMPARISON_FUNC DepthFuncConversion[] = {
                D3D11_COMPARISON_NEVER,         //TF_NEVER = 0,
                D3D11_COMPARISON_LESS,          //TF_LESS,            //! a< b
                D3D11_COMPARISON_EQUAL,         //TF_EQUAL,           //! a==b
                D3D11_COMPARISON_LESS_EQUAL,    //TF_LEQUAL,          //! a<=b
                D3D11_COMPARISON_GREATER,       //TF_GREATER,         //! a> b
                D3D11_COMPARISON_NOT_EQUAL,     //TF_NOTEQUAL,        //! a!=b
                D3D11_COMPARISON_GREATER_EQUAL, //TF_GEQUAL,          //! a>=b
                D3D11_COMPARISON_ALWAYS,        //TF_ALWAYS,
            };

            DepthStateID dsStateIndex = makeDepthStateIndex(flag, funcFlag);
            ID3D11DepthStencilState *foundState = mDepthStencilStates.Find(dsStateIndex);
            if (foundState == NULL) {
                ID3D11DepthStencilState *newState = NULL;
                // Make depth state
                D3D11_DEPTH_STENCIL_DESC depthDesc;
                ::memset(&depthDesc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
                depthDesc.StencilEnable = false;
                depthDesc.DepthFunc = DepthFuncConversion[static_cast<uint32>(funcFlag)];
                if (flag&DepthStateTest) {
                    depthDesc.DepthEnable = true;
                }
                else {
                    depthDesc.DepthEnable = false;
                }
                if (flag&DepthStateWrite) {
                    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
                }
                else {
                    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
                }
                mD3DDevice->CreateDepthStencilState(&depthDesc, &newState);
                mDepthStencilStates[dsStateIndex] = newState;

                return newState;
            }
            return foundState;
        }
        ID3D11BlendState* getBlendState(const BlendStateID &id)
        {
            ID3D11BlendState *foundState = mBlendStates.Find(id);

            if (foundState == NULL) {
                ID3D11BlendState *newState = NULL;
                // Make blend state
                D3D11_BLEND_DESC blendDesc;
                ::memset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));
                blendDesc.AlphaToCoverageEnable = true;
                blendDesc.IndependentBlendEnable = true;
                for (uint32 rtidx = 0; rtidx < RenderTargetMaxCount; rtidx++) {
                    blendDesc.RenderTarget[rtidx].BlendEnable = true;
                    blendDesc.RenderTarget[rtidx].SrcBlend = id.sourceColor[rtidx];
                    blendDesc.RenderTarget[rtidx].DestBlend = id.destColor[rtidx];
                    blendDesc.RenderTarget[rtidx].BlendOp = id.blendColor[rtidx];
                    blendDesc.RenderTarget[rtidx].SrcBlendAlpha = id.sourceAlpha[rtidx];
                    blendDesc.RenderTarget[rtidx].DestBlendAlpha = id.destAlpha[rtidx];
                    blendDesc.RenderTarget[rtidx].BlendOpAlpha = id.blendAlpha[rtidx];
                    blendDesc.RenderTarget[rtidx].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
                }
                HRESULT hr = mD3DDevice->CreateBlendState(&blendDesc, &newState);
                mBlendStates[id] = newState;

                return newState;
            }
            return foundState;
        }
    private:
        ID3D11Device            *mD3DDevice;
        ID3D11DeviceContext		*mD3DDeviceContext;
        ID3D11SamplerState      *mDefaultSampler;
        ID3D11RenderTargetView	*mBaseRenderTargetView;                         // View for rendertarget
        ID3D11DepthStencilView	*mBaseDepthStencilView;                         // View for depthstencil
        ID3D11RasterizerState   *mRasterizerStates[static_cast<uint32>(RendererFlag::Culling::Max)];
    };
}