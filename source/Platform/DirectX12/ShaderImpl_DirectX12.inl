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
@file  LE_ShaderImpl_DirectX12.inl
@brief Shader Implement (DX12)
@author minseob (leeminseob@outlook.com)
**********************************************************************/
namespace LimitEngine {
class ConstantBufferImpl_DirectX12 : public ConstantBufferImpl
{
public:
    ConstantBufferImpl_DirectX12() {}
    virtual ~ConstantBufferImpl_DirectX12() {}

    virtual void Create(Shader* InShader) override
    {}

    virtual void PrepareForDrawing() override
    {}

    virtual void Set(uint32 ShaderType, uint32 BufferIndex, uint32 Offset, const void* Data, size_t Size) override
    {}

    template<typename T>
    void Set(uint32 ShaderType, uint32 BufferIndex, uint32 Offset, T* Data)
    {
        this->Set(ShaderType, BufferIndex, Offset, Data, sizeof(T));
    }
};
class ShaderImpl_DirectX12 : public ShaderImpl
{
public:
    ShaderImpl_DirectX12()
    {
        ::memset(mShaderBinaries, 0, sizeof(mShaderBinaries));
    }
    virtual ~ShaderImpl_DirectX12()
    {
        for (uint32 TypeIndex = 0; TypeIndex < Shader::TYPE_NUM; TypeIndex++) {
            if (mShaderBinaries[TypeIndex]) {
                free(mShaderBinaries[TypeIndex]);
            }
        }
    }

    virtual bool PrepareForDrawing() override
    {
        return true;
    }

    virtual bool Compile(const char *code, int type) override
    { 
        /* Do nothing */ 
        return true;
    }

    virtual bool SetCompiledBinary(const unsigned char *bin, size_t size, int type) override
    {
        if (mShaderBinaries[type]) {
            free(mShaderBinaries[type]);
        }
        mShaderBinaries[type] = malloc(size);
        memcpy(mShaderBinaries[type], bin, size);

        return true;
    }

    virtual bool Link() override
    {
        /* Do nothing */
        return true;
    }

    virtual void Bind() override
    {
        UNIMPLEMENTED_ERROR
    }

    virtual int GetAttribPosition(const char *name) const override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }
    virtual int GetAttribPosition(uint32 type) const override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }
    virtual int GetUniformLocation(const char* name) const override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }
    virtual int GetParameterLocation(int loc_id) const override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }
    virtual int GetTextureLocation(const char* name) const override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }
    virtual int GetTextureLocation(int loc_id) const override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }
    virtual void SetUniformTexture(const char* name, uint32 n) override
    {
        UNIMPLEMENTED_ERROR
    }
    virtual void SetUniformParameter(const char* name, uint32 n) override
    {
        UNIMPLEMENTED_ERROR
    }

    virtual void SetUniformFloat1(ConstantBuffer* Buffer, int loc, const float& f) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &f);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformFloat2(ConstantBuffer* Buffer, int loc, const LEMath::FloatVector2 &v) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &v);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformFloat3(ConstantBuffer* Buffer, int loc, const LEMath::FloatVector3& v) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &v);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformFloat4(ConstantBuffer* Buffer, int loc, const LEMath::FloatVector4& v) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &v);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformInt1(ConstantBuffer* Buffer, int loc, const int32& n) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &n);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformInt2(ConstantBuffer* Buffer, int loc, const LEMath::IntVector2 &n) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &n);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformInt3(ConstantBuffer* Buffer, int loc, const LEMath::IntVector3& n) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &n);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformInt4(ConstantBuffer* Buffer, int loc, const LEMath::IntVector4& n) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, &n);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }
    virtual void SetUniformMatrix4(ConstantBuffer* Buffer, int loc, int size, float* f) override
    {
        if (ConstantBufferImpl_DirectX12* impl = (ConstantBufferImpl_DirectX12*)Buffer->GetImplementation()) {
            impl->Set(0, 0, loc, f, size);
        }

        if (mConstantBuffers.IndexOf(Buffer) < 0) {
            mConstantBuffers.Add(Buffer);
        }
    }

    virtual void* GetInputLayout(void* device, uint32 flag) override
    {
        UNIMPLEMENTED_ERROR
        return nullptr;
    }

    virtual int ConvertType(int type) override
    {
        UNIMPLEMENTED_ERROR
        return -1;
    }

private:
    void* mShaderBinaries[Shader::TYPE_NUM];
    VectorArray<ConstantBuffer*> mConstantBuffers;
};
}