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
@file ShaderManager.h
@brief ShaderManager
@author minseob (leeminseob@outlook.com)
**********************************************************************/

#include "Managers/ShaderManager.h"

#include "Managers/DrawManager.h"
#include "Managers/ShaderDriverManager.h"

//#include "Shaders/Shader_Draw2D.vs.h"
//#include "Shaders/Shader_ResolveSceneColorSRGB.ps.h"

#if 0
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
static const unsigned char Shader_StandardParamOnly_BasePass_PS[] = {
#include "shader/DirectX11/bin/StandardParamOnly.BasePass.ps.txt"
};
static const unsigned char Shader_StandardParamOnly_TranslucencyPass_PS[] = {
#include "shader/DirectX11/bin/StandardParamOnly.TranslucencyPass.ps.txt"
};
static const unsigned char Shader_OcclusionOnly_PrePass_PS[] = {
#include "shader/DirectX11/bin/OcclusionOnly.PrePass.ps.txt"
};
static const unsigned char Shader_OcclusionOnly_BasePass_PS[] = {
#include "shader/DirectX11/bin/OcclusionOnly.BasePass.ps.txt"
};
#elif defined(USE_DX12)
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
static const unsigned char Shader_StandardParamOnly_BasePass_PS[] = {
#include "shader/DirectX11/bin/StandardParamOnly.BasePass.ps.txt"
};
static const unsigned char Shader_StandardParamOnly_TranslucencyPass_PS[] = {
#include "shader/DirectX11/bin/StandardParamOnly.TranslucencyPass.ps.txt"
};
static const unsigned char Shader_OcclusionOnly_PrePass_PS[] = {
#include "shader/DirectX11/bin/OcclusionOnly.PrePass.ps.txt"
};
static const unsigned char Shader_OcclusionOnly_BasePass_PS[] = {
#include "shader/DirectX11/bin/OcclusionOnly.BasePass.ps.txt"
};
#else
#error No implementation for shaders
#endif
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
/*
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

        // Set StandardParamOnly shader (PrePass)
        {
            shader = new Shader("StandardParamOnly.PrePass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_Standard_PrePass_PS, sizeof(Shader_Standard_PrePass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set StandardParamOnly shader (BasePass)
        {
            shader = new Shader("StandardParamOnly.BasePass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_StandardParamOnly_BasePass_PS, sizeof(Shader_StandardParamOnly_BasePass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set StandardParamOnly shader (TranslucencyPass)
        {
            shader = new Shader("StandardParamOnly.TranslucencyPass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_StandardParamOnly_TranslucencyPass_PS, sizeof(Shader_StandardParamOnly_TranslucencyPass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set StandardParamOnly shader (PrePass)
        {
            shader = new Shader("OcclusionOnly.PrePass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_OcclusionOnly_PrePass_PS, sizeof(Shader_OcclusionOnly_PrePass_PS), Shader::TYPE_PIXEL);
            AddShader(shader);
        }

        // Set StandardParamOnly shader (BasePass)
        {
            shader = new Shader("OcclusionOnly.BasePass");
            shader->SetCompiledBinary(Shader_Standard_VS, sizeof(Shader_Standard_VS), Shader::TYPE_VERTEX);
            shader->SetCompiledBinary(Shader_OcclusionOnly_BasePass_PS, sizeof(Shader_OcclusionOnly_BasePass_PS), Shader::TYPE_PIXEL);
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
*/
        // Set Resolve scene color (SRGB)
        //{
        //    shader = new Shader("ResolveSceneColorSRGB");
        //    shader->SetCompiledBinary(Shader_Draw2D_VS, sizeof(Shader_Draw2D_VS), Shader::TYPE_VERTEX);
        //    shader->SetCompiledBinary(Shader_ResolveSceneColorSRGB_PS, sizeof(Shader_ResolveSceneColorSRGB_PS), Shader::TYPE_PIXEL);
        //    AddShader(shader);
        //}
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