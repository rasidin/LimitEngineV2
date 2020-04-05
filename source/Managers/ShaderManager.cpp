/*****************************************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file    ShaderManager.cpp
 * @brief    Shader Manager
 * @author    minseob
 *****************************************************************************/

#include "Managers/ShaderManager.h"

#ifdef USE_DX9
static const unsigned char Shader_Draw2D_VS[] = {
#include "shader/DirectX9/bin/Draw2D.vs.txt"
};
static const unsigned char Shader_Draw2D_PS[] = {
#include "shader/DirectX9/bin/Draw2D.ps.txt"
};
static const unsigned char Shader_DrawIBL_PS[] = {
#include "Shader/DirectX9/bin/DrawIBL.ps.txt"
};
static const unsigned char Shader_MakeIBLSpecularMipmap_PS[] = {
#include "Shader/DirectX9/bin/MakeIBLSpecularMipmap.ps.txt"
};
static const unsigned char Shader_GenerateBakedBRDF_CS[] = {
#include "Shader/DirectX9/bin/GenerateBakedBRDF.cs.txt"
};
#elif defined(USE_DX11)
static const unsigned char Shader_Draw2D_VS[] = {
#include "shader/DirectX11/bin/Draw2D.vs.txt"
};
static const unsigned char Shader_Draw2D_PS[] = {
#include "shader/DirectX11/bin/Draw2D.ps.txt"
};
//static const unsigned char Shader_DrawIBL_PS[] = {
//#include "shader/DirectX11/bin/DrawIBL.ps.txt"
//};
//static const unsigned char Shader_MakeIBLSpecularMipmap_PS[] = {
//#include "shader/DirectX11/bin/MakeIBLSpecularMipmap.ps.txt"
//};
//static const unsigned char Shader_GenerateBakedBRDF_CS[] = {
//#include "shader/DirectX11/bin/GenerateBakedBRDF.cs.txt"
//};
static const unsigned char Shader_DrawFont_PS[] = {
#include "shader/DirectX11/bin/DrawFont.ps.txt"
};
#endif

namespace LimitEngine {
#ifdef WINDOWS
    ShaderManager* SingletonShaderManager::mInstance = nullptr;
#endif

    ShaderManager::ShaderManager()
        : mShaderID(0)
    {
    }
    ShaderManager::~ShaderManager()
    {
        Term();
    }
    void ShaderManager::Init()
    {
        Shader *shader;

        // Set Sprite Shader
        {
            shader = new Shader("Draw2D");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_Draw2D_PS, sizeof(Shader_Draw2D_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set Font Shader
        {
            shader = new Shader("DrawFont");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_DrawFont_PS, sizeof(Shader_DrawFont_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

//        // Set DrawIBL Shader
//        {
//            shader = new Shader("DrawIBL");
//
//#ifdef WINDOWS
//            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
//            shader->SetCompiledBinary(Shader_DrawIBL_PS, sizeof(Shader_DrawIBL_PS), Shader::TYPE_PIXEL);
//#else
//            shader->Compile("<ShaderPath>Draw2D.vsh", Shader::TYPE_VERTEX);
//            shader->Compile("<ShaderPath>DrawIBL.fsh", Shader::TYPE_PIXEL);
//#endif
//            shader->Link();
//
//            AddShader(shader);
//        }
//
//        // Set DrawIBL Shader
//        {
//            shader = new Shader("MakeIBLSpecularMipmap");
//
//#ifdef WINDOWS
//            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
//            shader->SetCompiledBinary(Shader_MakeIBLSpecularMipmap_PS, sizeof(Shader_MakeIBLSpecularMipmap_PS), Shader::TYPE_PIXEL);
//#else
//            shader->Compile("<ShaderPath>Draw2D.vsh", Shader::TYPE_VERTEX);
//            shader->Compile("<ShaderPath>MakeIBLSpecularMipmap.fsh", Shader::TYPE_PIXEL);
//#endif
//            shader->Link();
//
//            AddShader(shader);
//        }
//
//		// Set GenerateBakedBRDF Shader
//		{
//			shader = new Shader("GenerateBakedBRDF");
//#ifdef WINDOWS
//			shader->SetCompiledBinary(Shader_GenerateBakedBRDF_CS, sizeof(Shader_GenerateBakedBRDF_CS), Shader::TYPE_COMPUTE);
//#else
//			shader->Compile("<ShaderPath>GenerateBakedBRDF.csh", Shader::TYPE_COMPUTE);
//#endif
//			AddShader(shader);
//
//		}
    }
    void ShaderManager::Term()
    {
        for(size_t i=0;i<mShaders.GetSize();i++)
        {
            delete mShaders[i];
        }
        mShaders.Clear();
    }
    void ShaderManager::BindShader(Shader *sh)
    {
        if (sh) sh->Bind();
    }
    void ShaderManager::BindShader(uint32 shaderID)
    {
        for(size_t i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetID() == shaderID)
            {
                BindShader(mShaders[i]);
                break;
            }
        }
    }
    void ShaderManager::BindShader(const char *name)
    {
        for(size_t i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetName() == name)
            {
                BindShader(mShaders[i]);
                break;
            }
        }
    }
    void ShaderManager::AddShader(Shader *sh)
    {
        for(size_t shidx=0;shidx<mShaders.GetSize();shidx++)
        {
            if (mShaders[shidx]->GetName() == sh->GetName()) {
                delete mShaders[shidx];
                mShaders.Delete(shidx);
                break;
            }
        }
        sh->SetID(mShaderID++);
        mShaders.push_back(sh);
    }
    uint32 ShaderManager::GetShaderID(const char *shaderName)
    {
        for(size_t i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetName() == shaderName)
            {
                return mShaders[i]->GetID();
            }
        }
        return -1;
    }
    Shader* ShaderManager::GetShader(const char *name)
    {
        for(size_t i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetName() == name)
            {
                return mShaders[i];
            }
        }
        return nullptr;
    }
}