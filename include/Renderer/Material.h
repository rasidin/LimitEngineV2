/*******************************************************************
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
@file  Material.h
@brief Material Class
@author minseob (leeminseob@outlook.com)
**********************************************************************/

#pragma once

#include <LERenderer>

#include <LEFloatVector4.h>

#include "Core/Object.h"
#include "Core/String.h"
#include "Core/TextParser.h"
#include "Renderer/Texture.h"
#include "Containers/MapArray.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/ShaderParameter.h"

#include "../externals/rapidxml/rapidxml.hpp"

namespace LimitEngine {
    class Shader;
    class RendererTask_MaterialSetupShaderParameters;
    class Material : public Object<LimitEngineMemoryCategory::Graphics>
    {
        friend RendererTask_MaterialSetupShaderParameters;
    public:
        Material();
        virtual ~Material();
        
        Material* Load(TextParser::NODE *root);
        Material* Load(rapidxml::xml_node<const char> *XMLNode);
        void SetupShaderParameters();
        void Bind(const RenderState &rs);

        void SetID(const String &n) { mId = n; }
        const String& GetID() const { return mId; }
        void SetName(const String &name) { mName = name; }
        const String& GetName() const { return mName; }
        
        bool IsEnabledRenderPass(const RenderPass& InRenderPass) const;
        void SetEnabledRenderPass(const RenderPass& InRenderPass, bool InEnabled) { mIsEnabledRenderPass[(uint32)InRenderPass] = InEnabled; }

		void SetShader(const RenderPass &InRenderPass, Shader *shader) { mShader[(uint32)InRenderPass] = shader; setupShaderParameters(); }
        ShaderRefPtr GetShader(const RenderPass &InRenderPass) const { return mShader[(uint32)InRenderPass]; }

        ConstantBufferRefPtr GetConstantBuffer(const RenderPass &InRenderPass) const { return mConstantBuffer[(uint32)InRenderPass]; }

		void SetParameter(const String &name, const ShaderParameter &param) { mParameters.Add(name, param); }
		ShaderParameter GetParameter(const String &name) const { return mParameters[name]; }
    private:
        void setupShaderParameters();

    private:
        String                              mId;
        String                              mName;

        bool                                mIsEnabledRenderPass[(uint32)RenderPass::NumOfRenderPass];

        MapArray<String, ShaderParameter>   mParameters;

        ShaderRefPtr                        mShader[(uint32)RenderPass::NumOfRenderPass];
        ConstantBufferRefPtr                mConstantBuffer[(uint32)RenderPass::NumOfRenderPass];

        friend Archive;
    };
}
