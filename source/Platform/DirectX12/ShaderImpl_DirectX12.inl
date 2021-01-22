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
class ShaderImpl_DirectX12 : public ShaderImpl
{
public:
    ShaderImpl_DirectX12()
    {
    }
    virtual ~ShaderImpl_DirectX12()
    {
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
};
}