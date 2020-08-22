/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Shader.h
 @brief Shader Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/19 Created by minseob
 ***********************************************************/

#ifndef _LE_SHADER_H_
#define _LE_SHADER_H_

#include <LERenderer>

#include <LEFloatVector2.h>
#include <LEFloatVector4.h>

#include "Core/Common.h"
#include "Core/ReferenceCountedObject.h"
#include "Core/String.h"
#include "Core/Mutex.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
    class ConstantBufferImpl : public Object<LimitEngineMemoryCategory_Graphics>
    {public:
        ConstantBufferImpl() {}
        virtual ~ConstantBufferImpl() {}

        virtual void Create(Shader *InShader) = 0;
        virtual void PrepareForDrawing() = 0;

        virtual void Set(uint32 ShaderType, uint32 BufferIndex, uint32 Offset, const void *Data, size_t Size) = 0;
    };
    class ConstantBuffer : public ReferenceCountedObject<LimitEngineMemoryCategory_Graphics>
    {
    public:
        ConstantBuffer();
        virtual ~ConstantBuffer();

        ConstantBufferImpl* GetImplementation() { return mImpl; }

        void Create(class Shader *InShader);
        void PrepareForDrawing();
    private:
        ConstantBufferImpl *mImpl;
    };

    class Texture;
    class ShaderImpl : public Object<LimitEngineMemoryCategory_Graphics>
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

        virtual void SetUniformFloat1 (ConstantBuffer *Buffer, int loc, const float &f) = 0;
        virtual void SetUniformFloat2 (ConstantBuffer *Buffer, int loc, const LEMath::FloatVector2 &v) = 0;
        virtual void SetUniformFloat3 (ConstantBuffer *Buffer, int loc, const LEMath::FloatVector3 &v) = 0;
        virtual void SetUniformFloat4 (ConstantBuffer *Buffer, int loc, const LEMath::FloatVector4 &v) = 0;
        virtual void SetUniformInt1   (ConstantBuffer *Buffer, int loc, const int32 &n) = 0;
        virtual void SetUniformInt2   (ConstantBuffer *Buffer, int loc, const LEMath::IntVector2 &n) = 0;
        virtual void SetUniformInt3   (ConstantBuffer *Buffer, int loc, const LEMath::IntVector3 &n) = 0;
        virtual void SetUniformInt4   (ConstantBuffer *Buffer, int loc, const LEMath::IntVector4 &v) = 0;
        virtual void SetUniformMatrix4(ConstantBuffer *Buffer, int loc, int size, float *f) = 0;

        virtual void* GetInputLayout(void *device, uint32 flag) = 0;
        
        virtual int ConvertType(int type) = 0;
    };
	class ShaderFactory;
    class RendererTask_CompileShader;
    class RendererTask_SetCompiledShader;
    class Shader : public ReferenceCountedObject<LimitEngineMemoryCategory_Graphics>
    {
		friend ShaderFactory;
        friend RendererTask_CompileShader;
        friend RendererTask_SetCompiledShader;
    public:
        enum TYPE
        {
            TYPE_VERTEX = 0,
            TYPE_PIXEL,
			TYPE_COMPUTE,
        };
        enum ShaderParameter
        {
            ShaderParameterDiffuse = 0,
            ShaderParameterNormal,
            ShaderParameterSpecular,
            ShaderParameterMax = 0xf
        };
        enum ShaderTexture
        {
            ShaderTextureDiffuse = 0,   // Diffuse
            ShaderTextureNormal,        // Normal
            ShaderTextureSpecular,      // Specular
            ShaderTextureMax = 0xf
        };
        
    public:
        Shader();
		Shader(const char *name);
        virtual ~Shader();
        
        ShaderImpl* GetImplementation() { return mImpl; }

        bool Compile(const char *filename, TYPE type);
        bool SetCompiledBinary(const unsigned char *bin, size_t size, TYPE type);
        bool Link()                     { if (mImpl) return mImpl->Link(); return false; }
        void Bind();
        
		bool HasShader(TYPE type) const { return mShaderContains & (1<<type); }

        bool PrepareForDrawing() { return mImpl->PrepareForDrawing(); }

		void AddDriver(ShaderDriver *driver) { mDrivers.Add(driver); }
		uint32 GetDriverCount() const { return static_cast<uint32>(mDrivers.count()); }
		ShaderDriver* GetDriver(uint32 n) { return mDrivers[n]; }
		ShaderDriver* GetDriver(const char *name);
		void ApplyDrivers(const RenderState &rs, Material *mat);

		void SetID(uint32 n)			{ _id = n; }
        uint32 GetID()					{ return _id; }
	    void SetName(const char *name)	{ _name = name; }
		const String&	GetName() const	{ return _name; }
        int  GetAttribPosition(const char *name) const
        { if (mImpl) return mImpl->GetAttribPosition(name); return -1; }
        int  GetAttribPosition(uint32 type) const
        { if (mImpl) return mImpl->GetAttribPosition(type); return  -1; }
        int  GetUniformLocation(const char *name) const
        { if (mImpl) return mImpl->GetUniformLocation(name); return -1; }
        int  GetParameterLocation(uint32 loc_id) const
        { if (mImpl) return mImpl->GetParameterLocation(loc_id); return -1; }
		int  GetTextureLocation(const char *name) const
		{ if (mImpl) return mImpl->GetTextureLocation(name); return -1; }
        int  GetTextureLocation(uint32 loc_id) const
        { if (mImpl) return mImpl->GetTextureLocation(loc_id); return -1; }
        void SetUniformParameter(const char *name, uint32 n)
        { if (mImpl) mImpl->SetUniformParameter(name, n); }
        void SetUniformTexture(const char *name, uint32 n)
        { if (mImpl) mImpl->SetUniformTexture(name, n); }
        void SetUniformFloat1(ConstantBuffer *buf, int location, const float v)
        { if (mImpl) mImpl->SetUniformFloat1(buf, location, v); }
        void SetUniformFloat2(ConstantBuffer *buf, int location, const LEMath::FloatVector2& v)
        { if (mImpl) mImpl->SetUniformFloat2(buf, location, v); }
        void SetUniformFloat3(ConstantBuffer *buf, int location, const LEMath::FloatVector3& v)
        { if (mImpl) mImpl->SetUniformFloat3(buf, location, v); }
        void SetUniformFloat4(ConstantBuffer *buf, int location, const LEMath::FloatVector4& v)
        { if (mImpl) mImpl->SetUniformFloat4(buf, location, v); }
        void SetUniformInt1(ConstantBuffer *buf, int location, const int32 v)
        { if (mImpl) mImpl->SetUniformInt1(buf, location, v); }
        void SetUniformInt2(ConstantBuffer *buf, int location, const LEMath::IntVector2 &v)
        { if (mImpl) mImpl->SetUniformInt2(buf, location, v); }
        void SetUniformInt3(ConstantBuffer *buf, int location, const LEMath::IntVector3 &v)
        { if (mImpl) mImpl->SetUniformInt3(buf, location, v); }
        void SetUniformInt4(ConstantBuffer *buf, int location, const LEMath::IntVector4 &v)
        { if (mImpl) mImpl->SetUniformInt4(buf, location, v); }
        void SetUniformMatrix4(ConstantBuffer *buf, int location, int size, float *pointer)
        { if (mImpl) mImpl->SetUniformMatrix4(buf, location, size, pointer); }
        void* GetInputLayout(void *device, uint32 flag) const
        { return mImpl?mImpl->GetInputLayout(device, flag):nullptr; }

	private:			// Private Functions
		void init();
		void setupDriver(const char *code);
        bool compileCode(const char *code, TYPE type);

		void checkShaderContains(TYPE type) { mShaderContains |= 1 << type; }

    private:
        uint32						 _id;           // ID from ShaderManager
        String						 _name;         // Shader Name
        ShaderImpl					*mImpl;         // Implement Unit
		VectorArray<ShaderDriver*>   mDrivers;		// Shader drivers

		uint32						 mShaderContains;
    };
}

#endif
