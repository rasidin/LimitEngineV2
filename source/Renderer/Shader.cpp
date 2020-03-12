/*****************************************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file	Shader.cpp
 * @brief	Shader Class
 * @author	minseob
 *****************************************************************************/

#include "Renderer/Shader.h"

#if    defined(USE_OPENGLES)
#include "OpenGLES/LE_ShaderImpl_OpenGLES.h"
#elif  defined(USE_DX9)
#include "DirectX9/LE_ShaderImpl_DirectX9.h"
#elif  defined(USE_DX11)
#include "../Platform/DirectX11/ShaderImpl_DirectX11.inl"
#else
#error No implementation of shader
#endif

#include "Core/Debug.h"
#include "Managers/DrawManager.h"
#include "Managers/ShaderDriverManager.h"
#include "Managers/ResourceManager.h"
#include "Renderer/ShaderDriver.h"

namespace LimitEngine {
    class RendererTask_CompileShader :public RendererTask
    {
    public:
        RendererTask_CompileShader(Shader *owner, void *bin, size_t size, Shader::TYPE type)
            : mOwner(owner)
            , mShaderCode(NULL)
            , mType(type)
        {
            mShaderCode = (char*)malloc(size);
            ::memcpy(mShaderCode, bin, size);
        }
        virtual ~RendererTask_CompileShader()
        {
            if (mShaderCode) {
                free(mShaderCode);
            }
            mShaderCode = NULL;
        }
        void Run() override
        {
            if (mOwner) {
                mOwner->compileCode(mShaderCode, mType);
            }
        }
    private:
        Shader *mOwner;
        char *mShaderCode;
        Shader::TYPE mType;
    };
    class RendererTask_SetCompiledShader : public RendererTask
    {
    public:
        RendererTask_SetCompiledShader(Shader *owner, const unsigned char *bin, size_t size, Shader::TYPE type)
            : mOwner(owner)
            , mBinary(bin)
            , mSize(size)
            , mType(type)
        {}
        void Run() override
        {
            mOwner->mImpl->SetCompiledBinary(mBinary, mSize, mType);
			mOwner->checkShaderContains(mType);
        }
    private:
        Shader *mOwner;
        const unsigned char *mBinary;
        size_t mSize;
        Shader::TYPE mType;
    };

#define SHADER_INIT		  mImpl(0)\
						, _id(0)\
						, mShaderContains(0u)

    Shader::Shader()
	    : SHADER_INIT
        , _name()
    {
		init();
    }
	Shader::Shader(const char *name)
		: SHADER_INIT
        , _name(name)
	{
		init();
	}
    Shader::~Shader()
    {
        if (mImpl) delete mImpl;
		mImpl = nullptr;

		for(uint32 sdidx=0;sdidx<mDrivers.count();sdidx++) {
			delete mDrivers[sdidx];
		}
		mDrivers.Clear();
    }
	void Shader::init()
	{
#if   defined(USE_OPENGLES)
        mImpl = new ShaderImpl_OpenGLES();
#elif defined(USE_DX11)
        mImpl = new ShaderImpl_DirectX11();
#elif defined(USE_DX9)
		mImpl = new ShaderImpl_DirectX9();
#endif
	}
	ShaderDriver* Shader::GetDriver(const char *name)
	{
		for(uint32 sdidx=0;sdidx<mDrivers.GetSize();sdidx++) {
			if(strcmp(mDrivers[sdidx]->GetName(), name) == 0)
				return mDrivers[sdidx];
		}
		return NULL;
	}
	void Shader::ApplyDrivers(const RenderState &rs, Material *mat)
	{
		for(uint32 sdidx=0;sdidx<mDrivers.GetSize();sdidx++) {
			mDrivers[sdidx]->Apply(rs, mat);
		}
	}
    bool Shader::Compile(const char *filename, TYPE type)
    {
        LEASSERT(mImpl);
        if (!mImpl) return false;

        gDebug << "Load Shader:" << filename;

        const ResourceManager::RESOURCE *resource = LE_ResourceManager.GetResourceWithRegister(filename);
        AutoPointer<RendererTask> rt_compileShader = new RendererTask_CompileShader(this, resource->data, resource->size, type);
        LE_DrawManager.AddRendererTask(rt_compileShader);

        gDebug << "... [ OK ]" << Debug::EndL;
        return true;
    }
	bool Shader::compileCode(const char *code, TYPE type)
	{
		if(code == NULL)
			return false;
		if (mImpl) {
			if(mImpl->Compile(code, int(type))) {
                setupDriver(code);
				checkShaderContains(type);
				return true;
            }
            else {
                return false;
            }
		}
        return true;
	}
    bool Shader::SetCompiledBinary(const unsigned char *bin, size_t size, TYPE type)
    {
        AutoPointer<RendererTask> rt_setCompiledShader = new RendererTask_SetCompiledShader(this, bin, size, type);
        LE_DrawManager.AddRendererTask(rt_setCompiledShader);
        return true;
    }
    void Shader::Bind()
    {
        LEASSERT(mImpl);
        if (!mImpl) return;
        
        mImpl->Bind();
    }
	void Shader::setupDriver(const char *code)
	{
		if(code == NULL) return;
		if(ShaderDriverManager *shaderDriverManager = ShaderDriverManager::GetSingletonPtr()) {
			shaderDriverManager->SetupShaderDriver(this, code);
		}
	}
}