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
@author minseob (https://github.com/rasidin)
**********************************************************************/

#include "Managers/ShaderManager.h"

#include "Managers/DrawManager.h"

#include "Shaders/Draw2D.vs.h"
#include "Shaders/Draw2D.ps.h"
#include "Shaders/DrawFullscreen.ps.h"
#include "Shaders/ResolveSceneColorSRGB.ps.h"
#include "Shaders/TemporalAA.ps.h"
#include "Shaders/Standard_prepass.vs.h"
#include "Shaders/Standard_prepass.ps.h"
#include "Shaders/Standard_basepass.vs.h"
#include "Shaders/Standard_basepass.ps.h"

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
        mShaders.Add(new Draw2D_VS());
        mShaders.Add(new Draw2D_PS());
        mShaders.Add(new DrawFullscreen_PS());
        mShaders.Add(new ResolveSceneColorSRGB_PS());
        mShaders.Add(new TemporalAA_PS());

        mShaders.Add(new Standard_prepass_VS());
        mShaders.Add(new Standard_prepass_PS());
        mShaders.Add(new Standard_basepass_VS());
        mShaders.Add(new Standard_basepass_PS());
    }
    void ShaderManager::Term()
    {
        for(uint32 i=0;i<mShaders.GetSize();i++)
        {
            mShaders[i] = nullptr;
        }
        mShaders.Clear();
    }
    
    Shader* ShaderManager::findshader(const ShaderHash& hash) const
    {
        for (uint32 shidx = 0; shidx < mShaders.count(); shidx++) {
            if (mShaders[shidx]->GetShaderHash() == hash) {
                return mShaders[shidx].Get();
            }
        }
        return nullptr;
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