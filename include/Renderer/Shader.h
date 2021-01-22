/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file Shader.h
@brief Shader
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#ifndef LIMITENGINEV2_RENDERER_SHADER_H_
#define LIMITENGINEV2_RENDERER_SHADER_H_

#include <LERenderer>

#include <LEFloatVector2.h>
#include <LEFloatVector4.h>

#include "Core/Common.h"
#include "Core/ReferenceCountedObject.h"
#include "Core/String.h"
#include "Core/Mutex.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
class Texture;
class ShaderImpl : public Object<LimitEngineMemoryCategory::Graphics>
{public:
    ShaderImpl() {}
    virtual ~ShaderImpl() {}
        
    virtual bool PrepareForDrawing() = 0;

    virtual bool Compile(const char *code, int type) = 0;
    virtual bool SetCompiledBinary(const unsigned char *bin, size_t size, int type) = 0;
    virtual bool Link() = 0;
    virtual void Bind() = 0;
    virtual int  GetAttribPosition(const char *name) const = 0;
    virtual int  GetAttribPosition(uint32 type) const = 0;
    virtual int  GetUniformLocation(const char *name) const = 0;
    virtual int  GetParameterLocation(int loc_id) const = 0;
    virtual int  GetTextureLocation(const char *name) const = 0;
    virtual int  GetTextureLocation(int loc_id) const = 0;
    virtual void SetUniformTexture(const char *name, uint32 n) = 0;
    virtual void SetUniformParameter(const char *name, uint32 n) = 0;

    virtual void* GetInputLayout(void *device, uint32 flag) = 0;

    virtual int ConvertType(int type) = 0;
};
class RendererTask_CompileShader;
class RendererTask_SetCompiledShader;

class ShaderHash
{
    union {
        uint8   data8[16];
        uint16  data16[8];
        uint32  data32[4];
        uint64  data64[2];
    };

public:
    ShaderHash(uint32 a, uint32 b, uint32 c, uint32 d) {
        data32[0] = a;
        data32[1] = b;
        data32[2] = c;
        data32[3] = d;
    }
    ShaderHash(const ShaderHash& src) {
        data64[0] = src.data64[0];
        data64[1] = src.data64[1];
    }
    ShaderHash& operator = (const ShaderHash& a) { data64[0] = a.data64[0]; data64[1] = a.data64[1]; return *this; }
    bool operator == (const ShaderHash& a) { return data64[0] == a.data64[0] && data64[1] == a.data64[1]; }

private:
    ShaderHash() { LEASSERT(false); } // empty data is prohibition
};
class Shader : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
    friend RendererTask_CompileShader;
    friend RendererTask_SetCompiledShader;
public:
    enum class Type : uint8
    {
        Vertex = 0,
        Pixel,
        Compute,
        Num
    };

public:
    Shader();
    virtual ~Shader();

    ShaderImpl* GetImplementation() { return mImpl; }
    bool PrepareForDrawing() { return mImpl->PrepareForDrawing(); }

    template<typename T>
    bool TypeOf() const { return GetShaderHash() == T::GetHash(); }

	void SetID(uint32 n)            { mId = n; }
    inline uint32 GetID() const     { return mId; }

    // Interfaces
    virtual       ShaderHash GetShaderHash() const { return ShaderHash{ 0,0,0,0 }; }
    virtual const String GetName() const = 0;
    virtual const uint8* GetCompiledCodeBin() const = 0;
    virtual const size_t GetCompiledCodeSize() const = 0;
    virtual const uint32 GetConstantBufferCount() const = 0;
    virtual const uint32 GetBoundTextureCount() const = 0;
    virtual const uint32 GetBoundSamplerCount() const = 0;
    virtual const uint32 GetUAVCount() const = 0;

private:
    uint32						 mId;           // ID from ShaderManager
    ShaderImpl					*mImpl;         // Implement Unit
};
} // namespace LimitEngine

#endif // LIMITENGINEV2_RENDERER_SHADER_H_
