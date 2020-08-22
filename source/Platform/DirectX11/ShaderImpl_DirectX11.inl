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
@file  LE_ShaderImpl_DirectX11.inl
@brief Shader Implement (DX11)
@author minseob (leeminseob@outlook.com)
**********************************************************************/
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
    enum ShaderType {
        SHADER_VS = 0,
        SHADER_PS,
        SHADER_CS,
        SHADER_COUNT,
    };

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
            ::memset(mConstantBufferSize, 0, sizeof(mConstantBufferSize));
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
        size_t                   mConstantBufferSize[ConstantBufferMaxCount];
        uint32					 mConstantBufferCount;
    private:
        void releaseCommon()
        {
            if (mReflection)
                mReflection->Release();
        }
    };
    class ShaderImpl_DirectX11 : public ShaderImpl
    {
        typedef uint32 FVFID;

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
                        mVertexShader.mConstantBufferCount = readyConstantBuffer(SHADER_VS, mVertexShader.mReflection, mVertexShader.mConstantBufferSize);
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
                        mPixelShader.mConstantBufferCount = readyConstantBuffer(SHADER_PS, mPixelShader.mReflection, mPixelShader.mConstantBufferSize);
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
                        mComputeShader.mConstantBufferCount = readyConstantBuffer(SHADER_PS, mComputeShader.mReflection, mComputeShader.mConstantBufferSize);
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
                    mVertexShader.mConstantBufferCount = readyConstantBuffer(SHADER_VS, mVertexShader.mReflection, mVertexShader.mConstantBufferSize);
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
                    mPixelShader.mConstantBufferCount = readyConstantBuffer(SHADER_PS, mPixelShader.mReflection, mPixelShader.mConstantBufferSize);
                }
                return true;
            case Shader::TYPE_COMPUTE:
                if (device->CreateComputeShader(bin, size, NULL, &mComputeShader.mShader) != S_OK) {
                    Debug::Error("[Shader]Fail to create compute shader");
                    return false;
                }
                if (D3DReflect(bin, size, IID_ID3D11ShaderReflection, (void**)&mComputeShader.mReflection) == S_OK) {
                    mComputeShader.mConstantBufferCount = readyConstantBuffer(SHADER_CS, mComputeShader.mReflection, mComputeShader.mConstantBufferSize);
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
        void SetUniformFloat1(ConstantBuffer *Buffer, int loc, const float &f) override
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &f, sizeof(float));
            }
        }
        void SetUniformFloat2(ConstantBuffer *Buffer, int loc, const LEMath::FloatVector2 &v) override
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &v, sizeof(LEMath::FloatVector2));
            }
        }
        void SetUniformFloat3(ConstantBuffer *Buffer, int loc, const LEMath::FloatVector3 &v) override
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &v, sizeof(LEMath::FloatVector3));
            }
        }
        void SetUniformFloat4(ConstantBuffer *Buffer, int loc, const LEMath::FloatVector4 &v) override
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &v, sizeof(LEMath::FloatVector4));
            }
        }
        void SetUniformInt1(ConstantBuffer *Buffer, int loc, const int32 &n) override
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;
            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &n, sizeof(int32));
            }
        }
        void SetUniformInt2(ConstantBuffer *Buffer, int loc, const LEMath::IntVector2 &v)
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &v, sizeof(LEMath::IntVector2));
            }
        }
        void SetUniformInt3(ConstantBuffer *Buffer, int loc, const LEMath::IntVector3 &v)
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &v, sizeof(LEMath::IntVector3));
            }
        }
        void SetUniformInt4(ConstantBuffer *Buffer, int loc, const LEMath::IntVector4 &v)
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, &v, sizeof(LEMath::IntVector4));
            }
        }
        void SetUniformMatrix4(ConstantBuffer *Buffer, int loc, int size, float *f) override
        {
            if (loc < 0 || loc >= static_cast<int>(mShaderVariables.GetSize()))
                return;

            ShaderVariable &variable = mShaderVariables[loc];
            if (ConstantBufferImpl *CBImpl = dynamic_cast<ConstantBufferImpl*>(Buffer->GetImplementation())) {
                CBImpl->Set(variable.shaderType, variable.constantBufferIndex, variable.variableDesc.StartOffset, f, sizeof(LEMath::FloatMatrix4x4));
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
        int readyConstantBuffer(ShaderType shType, ID3D11ShaderReflection *reflection, size_t *constantBufferSizes)
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

                constantBufferSizes[cbidx] = cbDesc.Size;
            }
            mShaderResources.Reserve(shaderDesc.BoundResources);
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

        friend class ConstantBufferImpl_DirectX11;
    };

    class ConstantBufferImpl_DirectX11 : public ConstantBufferImpl
    {
        struct ConstantBufferSet
        {
            static constexpr uint32 ConstantBufferMaxCount = 0x8;

            uint32           mConstantBufferCount;
            bool             mIsDirtyFlags[ConstantBufferMaxCount];
            ID3D11Buffer    *mConstantBuffers[ConstantBufferMaxCount];
            void            *mWritableConstantBuffers[ConstantBufferMaxCount];
            void Initialize() {
                mConstantBufferCount = 0u;
                ::memset(mIsDirtyFlags, 0, sizeof(mIsDirtyFlags));
                ::memset(mConstantBuffers, 0, sizeof(mConstantBuffers));
                ::memset(mWritableConstantBuffers, 0, sizeof(mWritableConstantBuffers));
            }
            void Create(uint32 InBufferCount, size_t* InBufferSizes) {
                ID3D11Device *device = reinterpret_cast<ID3D11Device*>(LE_DrawManager.GetDeviceHandle());
                LEASSERT(device);

                mConstantBufferCount = InBufferCount;
                for (uint32 bufIndex = 0; bufIndex < InBufferCount; bufIndex++) {
                    D3D11_BUFFER_DESC newCBDesc;
                    newCBDesc.ByteWidth = InBufferSizes[bufIndex];
                    newCBDesc.Usage = D3D11_USAGE_DYNAMIC;
                    newCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                    newCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                    newCBDesc.MiscFlags = 0;
                    newCBDesc.StructureByteStride = 0;
                    device->CreateBuffer(&newCBDesc, NULL, &mConstantBuffers[bufIndex]);

                    mWritableConstantBuffers[bufIndex] = MemoryAllocator::Alloc(InBufferSizes[bufIndex]);
                    ::memset(mWritableConstantBuffers[bufIndex], 0, InBufferSizes[bufIndex]);
                }
            }
            void Release() {
                for (uint32 cbIndex = 0; cbIndex < mConstantBufferCount; cbIndex++) {
                    if (mConstantBuffers[cbIndex])
                        mConstantBuffers[cbIndex]->Release();
                    if (mWritableConstantBuffers[cbIndex])
                        MemoryAllocator::Free(mWritableConstantBuffers[cbIndex]);
                }
                Initialize();
            }
            void Update() {
                if (mConstantBufferCount == 0) return;
       
                ID3D11DeviceContext *deviceContext = (ID3D11DeviceContext*)sDrawManager->GetDeviceContext();
                LEASSERT(deviceContext);
                
                for (uint32 cbIndex = 0; cbIndex < mConstantBufferCount; cbIndex++) {
                    if (mIsDirtyFlags[cbIndex] == false) continue;

                    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                    if (deviceContext->Map(mConstantBuffers[cbIndex], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource) == S_OK) {
                        ::memcpy(mappedSubResource.pData, mWritableConstantBuffers[cbIndex], mappedSubResource.RowPitch);
                        deviceContext->Unmap(mConstantBuffers[cbIndex], 0);
                    }
                }
            }
            void Bind(ShaderType InShaderType) {
                if (mConstantBufferCount == 0) return;

                ID3D11DeviceContext *deviceContext = (ID3D11DeviceContext*)sDrawManager->GetDeviceContext();
                LEASSERT(deviceContext);

                switch (InShaderType) {
                case SHADER_VS:
                    deviceContext->VSSetConstantBuffers(0, mConstantBufferCount, &mConstantBuffers[0]);
                    break;
                case SHADER_PS:
                    deviceContext->PSSetConstantBuffers(0, mConstantBufferCount, &mConstantBuffers[0]);
                    break;
                case SHADER_CS:
                    deviceContext->CSSetConstantBuffers(0, mConstantBufferCount, &mConstantBuffers[0]);
                    break;
                default: // Error!
                    LEASSERT(false);
                    break;
                }
            }
            void SetData(uint32 BufferIndex, uint32 Offset, const void *Src, size_t SizeOfData) {
                if (!mWritableConstantBuffers[BufferIndex]) return;

                uint8* Dst = (uint8*)mWritableConstantBuffers[BufferIndex] + Offset;
                if (::memcmp(Src, Dst, SizeOfData) != 0) {
                    mIsDirtyFlags[BufferIndex] = true;
                    ::memcpy(Dst, Src, SizeOfData);
                }
            }
        };
    public:
        ConstantBufferImpl_DirectX11() 
        {
            for (uint32 shIndex = 0, shCount = SHADER_COUNT; shIndex < shCount; shIndex++) {
                mConstantBufferSets[shIndex].Initialize();
            }
        }
        virtual ~ConstantBufferImpl_DirectX11() {}

        virtual void Create(Shader *InShader) override
        {
            if (ShaderImpl_DirectX11 *ShaderImpl = dynamic_cast<ShaderImpl_DirectX11*>(InShader->GetImplementation())) {
                for (uint32 shType = 0, shTypeCount = (uint32)SHADER_COUNT; shType < shTypeCount; shType++)
                {
                    switch ((ShaderType)shType)
                    {
                    case SHADER_VS:
                    {
                        mConstantBufferSets[shType].Create(ShaderImpl->mVertexShader.mConstantBufferCount, ShaderImpl->mVertexShader.mConstantBufferSize);
                    } break;
                    case SHADER_PS:
                    {
                        mConstantBufferSets[shType].Create(ShaderImpl->mPixelShader.mConstantBufferCount, ShaderImpl->mPixelShader.mConstantBufferSize);
                    } break;
                    case SHADER_CS:
                    {
                        mConstantBufferSets[shType].Create(ShaderImpl->mComputeShader.mConstantBufferCount, ShaderImpl->mComputeShader.mConstantBufferSize);
                    } break;
                    }
                }
            }
        }
        virtual void PrepareForDrawing() override
        {
            for (uint32 shIndex = 0, shCount = (uint32)SHADER_COUNT; shIndex < shCount; shIndex++) {
                mConstantBufferSets[shIndex].Update();
                mConstantBufferSets[shIndex].Bind((ShaderType)shIndex);
            }
        }

        virtual void Set(uint32 ShaderType, uint32 BufferIndex, uint32 Offset, const void *Data, size_t Size) override
        {
            mConstantBufferSets[ShaderType].SetData(BufferIndex, Offset, Data, Size);
        }
    private:
        ConstantBufferSet mConstantBufferSets[(uint32)SHADER_COUNT];
    };
}
#endif