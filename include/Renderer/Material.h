/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Material.h
 @brief Material Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#pragma once

#include <LERenderer>

#include <LEFloatVector4.h>

#include "Core/Object.h"
#include "Core/String.h"
#include "Core/TextParser.h"
#include "Renderer/Texture.h"
#include "Containers/MapArray.h"
#include "Renderer/ShaderDriverParameter.h"
#include "Renderer/ShaderParameter.h"

#include "rapidxml/rapidxml.hpp"

namespace LimitEngine {
    class Shader;
    class RendererTask_MaterialSetupShaderParameters;
    class Material : public Object<LimitEngineMemoryCategory_Graphics>
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
        
		void SetShader(const RenderPass &InRenderPass, Shader *shader) { mShader[(uint32)InRenderPass] = shader; setupShaderParameters(); }
        ShaderRefPtr GetShader(const RenderPass &InRenderPass) const { return mShader[(uint32)InRenderPass]; }

		void SetParameter(const String &name, const ShaderParameter &param) { mParameters.Add(name, param); }
		ShaderParameter GetParameter(const String &name) const { return mParameters[name]; }
    private:
        void setupShaderParameters();

    private:
        String                              mId;
        String                              mName;

        MapArray<String, ShaderParameter>   mParameters;
        ShaderDriverParameter              *mShaderDriverParameter;

        ShaderRefPtr                        mShader[(uint32)RenderPass::NumOfRenderPass];

        friend Archive;
    };
}
