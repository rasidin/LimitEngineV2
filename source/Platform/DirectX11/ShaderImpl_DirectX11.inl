/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  LE_ShaderImpl_DirectX11.inl
@brief Shader Implement (DX11)
@author minseob (leeminseob@outlook.com)
***********************************************************/
#ifdef USE_DX11
#include "Core/Debug.h"
#include "Containers/MapArray.h"
#include "Managers/DrawManager.h"
#include "Renderer/Texture.h"
#include "Renderer/Vertex.h"

#include <d3d11.h>
#include <d3d11Shader.h>
#include <d3dcompiler.h>

namespace LimitEngine
{
    template <typename T> struct ShaderData_DirectX11 : public Object<LimitEngineMemoryCategory_Graphics>
    {
        static const uint32 ConstantBufferMaxCount = 8;

        ShaderData_DirectX11()
            : mShader(nullptr)
            , mReflection(nullptr)
            , mConstantBufferCount(0u)
        {}
        void Initialize()
        {
            mShader = nullptr;
            mReflection = nullptr;
            mConstantBufferCount = 0u;
            ::memset(mConstantBuffer, 0, sizeof(mConstantBuffer));
            ::memset(mConstantBufferPointer, 0, sizeof(mConstantBufferPointer));
        }
        void Release()
        {
            if (mShader)
                mShader->Release();
            releaseCommon();
            Initialize();
        }

        T						*mShader;
        ID3D11ShaderReflection	*mReflection;
        ID3D11Buffer			*mConstantBuffer[ConstantBufferMaxCount];
        void					*mConstantBufferPointer[ConstantBufferMaxCount];
        uint32					 mConstantBufferCount;
    private:
        void releaseCommon()
        {
            if (mReflection)
                mReflection->Release();
            for (uint32 cbpidx = 0; cbpidx < mConstantBufferCount; cbpidx++) {
                if (mConstantBufferPointer[cbpidx]) {
                    free(mConstantBufferPointer[cbpidx]);
                }
            }
        }
    };
    class ShaderImpl_DirectX11 : public ShaderImpl
    {
        typedef uint32 FVFID;

        enum ShaderType {
            SHADER_VS = 0,
            SHADER_PS,
            SHADER_CS,
            SHADER_COUNT,
        };

        static const uint32 ConstantBufferCount = 8;

        struct ShaderVariable
        {
            char Name[0xff];
            ShaderType shaderType;
            uint32 constantBufferIndex;
            uint32 variableIndex;
            D3D11_SHADER_VARIABLE_DESC variableDesc;
        };

        struct ShaderResource
        {
            char Name[0xff];
            D3D11_SHADER_INPUT_BIND_DESC desc;
        };
    public:
        ShaderImpl_DirectX11()
            : mVertexShader()
            , mPixelShader()
            , mComputeShader()
            , mOriginalVSCodeData(nullptr)
            , mOriginalVSCodeDataSize(0u)
        {
            mVertexShader.Initialize();
            mPixelShader.Initialize();
            mComputeShader.Initialize();
        }
        virtual ~ShaderImpl_DirectX11()
        {
            if (mOriginalVSCodeData)
                free(mOriginalVSCodeData);

            mVertexShader.Release();
            mPixelShader.Release();
            mComputeShader.Release();

            for (uint32 ilidx = 0; ilidx < mVertexInputLayouts.GetSize(); ilidx++) {
                if (mVertexInputLayouts.GetAt(ilidx).value) {
                    mVertexInputLayouts.GetAt(ilidx).value->Release();
                    mVertexInputLayouts.GetAt(ilidx).value = NULL;
                }
            }
            mVertexInputLayouts.Clear();
            mOriginalVSCodeData = nullptr;
            mOriginalVSCodeDataSize = 0u;
        }

        bool PrepareForDrawing() override
        {
            ID3D11DeviceContext *deviceContext = (ID3D11DeviceContext*)sDrawManager->GetDeviceContext();
            LEASSERT(deviceContext);

            for (uint32 vcbidx = 0; vcbidx < mVertexShader.mConstantBufferCount; vcbidx++) {
                D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                if (deviceContext->Map(mVertexShader.mConstantBuffer[vcbidx], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource) == S_OK) {
                    ::memcpy(mappedSubResource.pData, mVertexShader.mConstantBufferPointer[vcbidx], mappedSubResource.RowPitch);
                    deviceContext->Unmap(mVertexShader.mConstantBuffer[vcbidx], 0);
                }
            }
            deviceContext->VSSetConstantBuffers(0, mVertexShader.mConstantBufferCount, &mVertexShader.mConstantBuffer[0]);
            for (uint32 pcbidx = 0; pcbidx < mPixelShader.mConstantBufferCount; pcbidx++) {
                D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                if (deviceContext->Map(mPixelShader.mConstantBuffer[pcbidx], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource) == S_OK) {
                    ::memcpy(mappedSubResource.pData, mPixelShader.mConstantBufferPointer[pcbidx], mappedSubResource.RowPitch);
                    deviceContext->Unmap(mPixelShader.mConstantBuffer[pcbidx], 0);
                }
            }
            deviceContext->PSSetConstantBuffers(0, mPixelShader.mConstantBufferCount, &mPixelShader.mConstantBuffer[0]);
            for (uint32 pcbidx = 0; pcbidx < mComputeShader.mConstantBufferCount; pcbidx++) {
                D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                if (deviceContext->Map(mComputeShader.mConstantBuffer[pcbidx], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource) == S_OK) {
                    ::memcpy(mappedSubResource.pData, mComputeShader.mConstantBufferPointer[pcbidx], mappedSubResource.RowPitch);
                    deviceContext->Unmap(mComputeShader.mConstantBuffer[pcbidx], 0);
                }
            }
            deviceContext->CSSetConstantBuffers(0, mComputeShader.mConstantBufferCount, &mComputeShader.mConstantBuffer[0]);
            return true;
        }

        bool Compile(const char *code, int type) override
        {
            LEASSERT(code);

            ID3DBlob *pCode = NULL;
            ID3DBlob *pError = NULL;
            ID3D11Device *device = reinterpret_cast<ID3D11Device*>(LE_DrawManager.GetDeviceHandle());
            LEASSERT(device);
            switch (type)
            {
            case Shader::TYPE_VERTEX:
            {
                if (D3DCompile(code, strlen(code), "VertexShader", NULL, NULL, "vs_main", "vs_4_0", 0, 0, &pCode, &pError) == S_OK) {
                    if (device->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &mVertexShader.mShader) != S_OK) {
                        return false;
                    }
                    if (D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&mVertexShader.mReflection) == S_OK) {
                        mVertexShader.mConstantBufferCount = readyConstantBuffer(SHADER_VS, mVertexShader.mReflection, mVertexShader.mConstantBuffer, mVertexShader.mConstantBufferPointer);
                    }
                    mOriginalVSCodeDataSize = pCode->GetBufferSize();
                    mOriginalVSCodeData = malloc(mOriginalVSCodeDataSize);
                    ::memcpy(mOriginalVSCodeData, pCode->GetBufferPointer(), pCode->GetBufferSize());
                    pCode->Release();
                }
                else {
                    Debug::Error("VertexShader compile error!!! : %s", pError->GetBufferPointer());
                    pError->Release();
                    return false;
                }
            } return true;
            case Shader::TYPE_PIXEL:
            {
                if (D3DCompile(code, strlen(code), "PixelShader", NULL, NULL, "ps_main", "ps_4_0", 0, 0, &pCode, &pError) == S_OK) {
                    if (device->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &mPixelShader.mShader) != S_OK) {
                        return false;
                    }
                    if (D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&mPixelShader.mReflection) == S_OK) {
                        mPixelShader.mConstantBufferCount = readyConstantBuffer(SHADER_PS, mPixelShader.mReflection, mPixelShader.mConstantBuffer, mPixelShader.mConstantBufferPointer);
                    }
                    pCode->Release();
                }
                else {
                    Debug::Error("PixelShader compile error!!! : %s", pError->GetBufferPointer());
                    pError->Release();
                    return false;
                }
            } return true;
            case Shader::TYPE_COMPUTE:
            {
                if (D3DCompile(code, strlen(code), "ComputeShader", NULL, NULL, "cs_main", "cs_4_0", 0, 0, &pCode, &pError) == S_OK) {
                    if (device->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &mComputeShader.mShader) != S_OK) {
                        return false;
                    }
                    if (D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&mComputeShader.mReflection) == S_OK) {
                        mComputeShader.mConstantBufferCount = readyConstantBuffer(SHADER_PS, mComputeShader.mReflection, mComputeShader.mConstantBuffer, mComputeShader.mConstantBufferPointer);
                    }
                    pCode->Release();
                }
                else {
                    Debug::Error("PixelShader compile error!!! : %s", pError->GetBufferPointer());
                    pError->Release();
                    return false;
                }
            } return true;
            default: return false;
            }
        }
        bool SetCompiledBinary(const unsigned char *bin, size_t size, int type) override
        {
            ID3D11Device *device = reinterpret_cast<ID3D11Device*>(LE_DrawManager.GetDeviceHandle());
            LEASSERT(device);
            switch (type)
            {
            case Shader::TYPE_VERTEX:
                if (device->CreateVertexShader(bin, size, NULL, &mVertexShader.mShader) != S_OK) {
                    Debug::Error("[Shader]Fail to create vertex shader");
                    return false;
                }
                if (D3DReflect(bin, size, IID_ID3D11ShaderReflection, (void**)&mVertexShader.mReflection) == S_OK) {
                    mVertexShader.mConstantBufferCount = readyConstantBuffer(SHADER_VS, mVertexShader.mReflection, mVertexShader.mConstantBuffer, mVertexShader.mConstantBufferPointer);
                }
                mOriginalVSCodeDataSize = size;
                mOriginalVSCodeData = malloc(mOriginalVSCodeDataSize);
                ::memcpy(mOriginalVSCodeData, bin, size);
                return true;
            case Shader::TYPE_PIXEL:
                if (device->CreatePixelShader(bin, size, NULL, &mPixelShader.mShader) != S_OK) {
                    Debug::Error("[Shader]Fail to create pixel shader");
                    return false;
                }
                if (D3DReflect(bin, size, IID_ID3D11ShaderReflection, (void**)&mPixelShader.mReflection) == S_OK) {
                    mPixelShader.mConstantBufferCount = readyConstantBuffer(SHADER_PS, mPixelShader.mReflection, mPixelShader.mConstantBuffer, mPixelShader.mConstantBufferPointer);
                }
                return true;
            case Shader::TYPE_COMPUTE:
                if (device->CreateComputeShader(bin, size, NULL, &mComputeShader.mShader) != S_OK) {
                    Debug::Error("[Shader]Fail to create compute shader");
                    return false;
                }
                if (D3DReflect(bin, size, IID_ID3D11ShaderReflection, (void**)&mComputeShader.mReflection) == S_OK) {
                    mComputeShader.mConstantBufferCount = readyConstantBuffer(SHADER_CS, mComputeShader.mReflection, mComputeShader.mConstantBuffer, mComputeShader.mConstantBufferPointer);
                }
                return true;
            default:
                return false;
            }
        }
        bool Link() override
        { // Do nothing...
            return true;
        }
        void Bind() override
        {
            ID3D11DeviceContext *devCtx = (ID3D11DeviceContext*)sDrawManager->GetDeviceContext();
            LEASSERT(devCtx);

            if (mVertexShader.mShader)
                devCtx->VSSetShader(mVertexShader.mShader, nullptr, 0);
            if (mPixelShader.mShader)
                devCtx->PSSetShader(mPixelShader.mShader, nullptr, 0);
            if (mComputeShader.mShader)
                devCtx->CSSetShader(mComputeShader.mShader, nullptr, 0);
        }
        int  GetAttribPosition(const char *name) const override
        { // unimplemented
            return -1;
        }
        int  GetAttribPosition(uint32 type) const override
        { // unimplemented
            return -1;
        }
        int  GetUniformLocation(const char *name) const override
        {
            for (uint32 varidx = 0; varidx < mShaderVariables.GetSize(); varidx++) {
                if (strcmp(mShaderVariables[varidx].Name, name) == 0) {
                    return varidx;
                }
            }
            return -1;
        }
        int  GetParameterLocation(int loc_id) const override
        { // unimplemented
            return -1;
        }
        int  GetTextureLocation(const char *name) const override
        {
            for (uint32 texidx = 0; texidx < mShaderResources.GetSize(); texidx++) {
                if (strcmp(mShaderResources[texidx].Name, name) == 0) {
                    return mShaderResources[texidx].desc.BindPoint;
                }
            }
            return -1;
        }
        int  GetTextureLocation(int loc_id) const override
        { // unimplemented
            return -1;
        }
        void SetUniformTexture(const char *name, uint32 n) override
        { // unimplemented
        }
        void SetUniformParameter(const char *name, uint32 n) override
        { // unimplemented
        }
        void SetUniformFloat1(int loc, const float &f) override
        {
            if (loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            switch (variable.shaderType)
            {
            case SHADER_VS:
                *(float*)(((uint8*)mVertexShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = f;
                break;
            case SHADER_PS:
                *(float*)(((uint8*)mPixelShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = f;
                break;
            case SHADER_CS:
                *(float*)(((uint8*)mComputeShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = f;
                break;
            default: break;
            }
        }
        void SetUniformFloat2(int loc, const LEMath::FloatVector2 &v) override
        {
            if (loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            switch (variable.shaderType)
            {
            case SHADER_VS:
                *(LEMath::FloatVector2*)(((uint8*)mVertexShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = v;
                break;
            case SHADER_PS:
                *(LEMath::FloatVector2*)(((uint8*)mPixelShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = v;
                break;
            case SHADER_CS:
                *(LEMath::FloatVector2*)(((uint8*)mComputeShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = v;
                break;
            default: break;
            }
        }
        void SetUniformFloat4(int loc, const LEMath::FloatVector4 &v) override
        {
            if (loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            switch (variable.shaderType)
            {
            case SHADER_VS:
                *(LEMath::FloatVector4*)(((uint8*)mVertexShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = v;
                break;
            case SHADER_PS:
                *(LEMath::FloatVector4*)(((uint8*)mPixelShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = v;
                break;
            case SHADER_CS:
                *(LEMath::FloatVector4*)(((uint8*)mComputeShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset) = v;
                break;
            default: break;
            }
        }
        void SetUniformMatrix4(int loc, int size, float *f) override
        {
            if (loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            switch (variable.shaderType)
            {
            case SHADER_VS:
                ::memcpy(((uint8*)mVertexShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset, f, sizeof(float) * 16);
                break;
            case SHADER_PS:
                ::memcpy(((uint8*)mPixelShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset, f, sizeof(float) * 16);
                break;
            case SHADER_CS:
                ::memcpy(((uint8*)mComputeShader.mConstantBufferPointer[variable.constantBufferIndex]) + variable.variableDesc.StartOffset, f, sizeof(float) * 16);
                break;
            default: break;
            }
        }
        int ConvertType(int type) override
        {
            return -1;
        }

        void* GetInputLayout(void *Device, uint32 Flag) override
        {
            if (Device == NULL)
                return NULL;

            ID3D11Device *device = (ID3D11Device*)Device;

            ID3D11InputLayout *output = mVertexInputLayouts.Find(Flag, NULL);
            if (output == NULL) { // Create
                ID3D11InputLayout *newIL = NULL;
                const char *sematicNames[] = {
                    "POSITION",
                    "NORMAL",
                    "COLOR",
                    "TEXCOORD",
                    "WEIGHT",
                    "TEXCOORD",
                    "TEXCOORD"
                };

                VectorArray<D3D11_INPUT_ELEMENT_DESC> ieDescs;
                uint32 currentOffset = 0;
                if (Flag & FVF_TYPE_POSITION) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[0];
                    ieDesc.SemanticIndex = 0;
                    ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_POSITION;
                }
                if (Flag & FVF_TYPE_NORMAL) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[1];
                    ieDesc.SemanticIndex = 0;
                    ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_NORMAL;
                }
                if (Flag & FVF_TYPE_COLOR) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[2];
                    ieDesc.SemanticIndex = 0;
                    ieDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_COLOR;
                }
                if (Flag & FVF_TYPE_TEXCOORD) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[3];
                    ieDesc.SemanticIndex = 0;
                    ieDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_TEXCOORD;
                }
                if (Flag & FVF_TYPE_WEIGHT) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[4];
                    ieDesc.SemanticIndex = 0;
                    ieDesc.Format = DXGI_FORMAT_R32_FLOAT;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_WEIGHT;
                }
                if (Flag & FVF_TYPE_TANGENT) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[5];
                    ieDesc.SemanticIndex = 1;
                    ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_TANGENT;
                }
                if (Flag & FVF_TYPE_BINORMAL) {
                    D3D11_INPUT_ELEMENT_DESC& ieDesc = ieDescs.Add();
                    ieDesc.SemanticName = sematicNames[6];
                    ieDesc.SemanticIndex = 2;
                    ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                    ieDesc.InputSlot = 0;
                    ieDesc.AlignedByteOffset = currentOffset;
                    ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    ieDesc.InstanceDataStepRate = 0;

                    currentOffset += FVF_SIZE_BINORMAL;
                }

                D3D11_INPUT_ELEMENT_DESC *descsPtr = &ieDescs.GetStart();
                if (FAILED(device->CreateInputLayout(descsPtr, static_cast<UINT>(ieDescs.GetSize()), mOriginalVSCodeData, mOriginalVSCodeDataSize, &newIL))) {
                    Debug::Error("[Shader]Fail to create input layout");
                    return NULL;
                }
                mVertexInputLayouts[Flag] = newIL;
                return newIL;
            }
            return output;
        }
    private:
        int readyConstantBuffer(ShaderType shType, ID3D11ShaderReflection *reflection, ID3D11Buffer **constantBuffers, void **constantBufferPointers)
        {
            ID3D11Device *device = reinterpret_cast<ID3D11Device*>(LE_DrawManager.GetDeviceHandle());
            LEASSERT(device);

            D3D11_SHADER_DESC shaderDesc;
            reflection->GetDesc(&shaderDesc);

            uint32 constantBufferCount = shaderDesc.ConstantBuffers;
            for (uint32 cbidx = 0; cbidx < constantBufferCount; cbidx++) {
                ID3D11ShaderReflectionConstantBuffer *constantBuffer = reflection->GetConstantBufferByIndex(cbidx);
                D3D11_SHADER_BUFFER_DESC cbDesc;
                constantBuffer->GetDesc(&cbDesc);
                for (uint32 varidx = 0; varidx < cbDesc.Variables; varidx++) {
                    ID3D11ShaderReflectionVariable *srvar = constantBuffer->GetVariableByIndex(varidx);
                    ShaderVariable &shaderVar = mShaderVariables.Add();
                    shaderVar.constantBufferIndex = cbidx;
                    shaderVar.shaderType = shType;

                    D3D11_SHADER_VARIABLE_DESC varDesc;
                    srvar->GetDesc(&varDesc);
                    shaderVar.variableDesc = varDesc;
                    ::memcpy(shaderVar.Name, varDesc.Name, strlen(varDesc.Name) + 1);
                    shaderVar.variableDesc.Name = shaderVar.Name;
                }

                D3D11_BUFFER_DESC newCBDesc;
                newCBDesc.ByteWidth = cbDesc.Size;
                newCBDesc.Usage = D3D11_USAGE_DYNAMIC;
                newCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                newCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                newCBDesc.MiscFlags = 0;
                newCBDesc.StructureByteStride = 0;
                device->CreateBuffer(&newCBDesc, NULL, &constantBuffers[cbidx]);

                constantBufferPointers[cbidx] = malloc(cbDesc.Size);
            }
            for (uint32 rbidx = 0; rbidx < shaderDesc.BoundResources; rbidx++) {
                D3D11_SHADER_INPUT_BIND_DESC ibDesc;
                reflection->GetResourceBindingDesc(rbidx, &ibDesc);

                ShaderResource &shaderRes = mShaderResources.Add();
                shaderRes.desc = ibDesc;
                ::memcpy(shaderRes.Name, ibDesc.Name, strlen(ibDesc.Name) + 1);
                shaderRes.desc.Name = shaderRes.Name;
            }
            return constantBufferCount;
        }
    private:
        ShaderData_DirectX11<ID3D11VertexShader>	mVertexShader;
        ShaderData_DirectX11<ID3D11PixelShader>		mPixelShader;
        ShaderData_DirectX11<ID3D11ComputeShader>	mComputeShader;

        VectorArray<ShaderVariable>					mShaderVariables;
        VectorArray<ShaderResource>					mShaderResources;

        MapArray<FVFID, ID3D11InputLayout*>			mVertexInputLayouts;

        void                                       *mOriginalVSCodeData;
        size_t										mOriginalVSCodeDataSize;
    };
}
#endif