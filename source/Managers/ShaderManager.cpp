/*****************************************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file    ShaderManager.cpp
 * @brief    Shader Manager
 * @author    minseob
 *****************************************************************************/

#include "Managers/ShaderManager.h"

#include "Managers/DrawManager.h"
#include "Managers/ShaderDriverManager.h"

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
static const unsigned char Shader_DrawFullscreen_PS[] = {
#include "shader/DirectX11/bin/DrawFullscreen.ps.txt"
};
static const unsigned char Shader_ResolveSceneColorSRGB_PS[] = {
#include "shader/DirectX11/bin/ResolveSceneColorSRGB.ps.txt"
};
static const unsigned char Shader_TemporalAA_PS[] = {
#include "shader/DirectX11/bin/TemporalAA.ps.txt"
};
static const unsigned char Shader_AmbientOcclusion_PS[] = {
#include "shader/DirectX11/bin/AmbientOcclusion.ps.txt"
};
static const unsigned char Shader_AmbientOcclusionBlur_PS[] = {
#include "shader/DirectX11/bin/AmbientOcclusionBlur.ps.txt"
};
static const unsigned char Shader_DrawFont_PS[] = {
#include "shader/DirectX11/bin/DrawFont.ps.txt"
};
static const unsigned char Shader_Standard_VS[] = {
#include "shader/DirectX11/bin/Standard.vs.txt"
};
static const unsigned char Shader_Standard_PrePass_PS[] = {
#include "shader/DirectX11/bin/Standard.PrePass.ps.txt"
};
static const unsigned char Shader_Standard_BasePass_PS[] = {
#include "shader/DirectX11/bin/Standard.BasePass.ps.txt"
};
static const unsigned char Shader_Standard_TranslucencyPass_PS[] = {
#include "shader/DirectX11/bin/Standard.TranslucencyPass.ps.txt"
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

        // Set Standard shader (PrePass)
        {
            shader = new Shader("Standard.PrePass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_Standard_PrePass_PS, sizeof(Shader_Standard_PrePass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set Standard shader (BasePass)
        {
            shader = new Shader("Standard.BasePass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_Standard_BasePass_PS, sizeof(Shader_Standard_BasePass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set Standard shader (TranslucencyPass)
        {
            shader = new Shader("Standard.TranslucencyPass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_Standard_TranslucencyPass_PS, sizeof(Shader_Standard_TranslucencyPass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

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

        // Set 
        {
            shader = new Shader("DrawFullscreen");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_DrawFullscreen_PS, sizeof(Shader_DrawFullscreen_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set Resolve scene color (SRGB)
        {
            shader = new Shader("ResolveSceneColorSRGB");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_ResolveSceneColorSRGB_PS, sizeof(Shader_ResolveSceneColorSRGB_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // PostProcess TemporalAA
        {
            shader = new Shader("TemporalAA");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_TemporalAA_PS, sizeof(Shader_TemporalAA_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // PostProcess AmbientOcclusion
        {
            shader = new Shader("AmbientOcclusion");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_AmbientOcclusion_PS, sizeof(Shader_AmbientOcclusion_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }
        // PostProcess AmbientOcclusion Blur
        {
            shader = new Shader("AmbientOcclusionBlur");
            shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_AmbientOcclusionBlur_PS, sizeof(Shader_AmbientOcclusionBlur_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }
    }
    void ShaderManager::Term()
    {
        for(uint32 i=0;i<mShaders.GetSize();i++)
        {
            mShaders[i] = nullptr;
        }
        mShaders.Clear();
    }
    void ShaderManager::BindShader(Shader *sh)
    {
        if (sh) sh->Bind();
    }
    void ShaderManager::BindShader(uint32 shaderID)
    {
        for(uint32 i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetID() == shaderID)
            {
                BindShader(mShaders[i].Get());
                break;
            }
        }
    }
    void ShaderManager::BindShader(const char *name)
    {
        for(uint32 i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetName() == name)
            {
                BindShader(mShaders[i].Get());
                break;
            }
        }
    }
    void ShaderManager::AddShader(Shader *sh)
    {
        for(uint32 shidx=0;shidx<mShaders.GetSize();shidx++)
        {
            if (mShaders[shidx]->GetName() == sh->GetName()) {
                mShaders[shidx] = nullptr;
                mShaders.Delete(shidx);
                break;
            }
        }
        Shader *CapturedShader = sh;
        ShaderDriverManager *CapturedManager = ShaderDriverManager::GetSingletonPtr();
        LE_DrawManager.AddRendererTaskLambda([CapturedManager, CapturedShader]() {
            CapturedManager->SetupShaderDriver(CapturedShader);
        });
        sh->SetID(mShaderID++);
        mShaders.push_back(sh);
    }
    uint32 ShaderManager::GetShaderID(const char *shaderName)
    {
        for(uint32 i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetName() == shaderName)
            {
                return mShaders[i]->GetID();
            }
        }
        return -1;
    }
    ShaderRefPtr ShaderManager::GetShader(const char *name)
    {
        for(uint32 i=0;i<mShaders.GetSize();i++)
        {
            if (mShaders[i]->GetName() == name)
            {
                return mShaders[i];
            }
        }
        return nullptr;
    }
}