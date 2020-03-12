/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Material.cpp
 @brief Material Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Managers/DrawManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderDriverParameter.h"

namespace LimitEngine {
    class RendererTask_MaterialSetupShaderParameters : public RendererTask
    {
    public:
        RendererTask_MaterialSetupShaderParameters(Material *owner)
            : mOwner(owner)
        {}
        void Run() override
        {
            if (mOwner)
                mOwner->setupShaderParameters();
        }
    private:
        Material *mOwner;
    };

    Material::Material()
        : mId()
        , mName()
        , mShader(NULL)
        , mShaderDriverParameter(NULL)
    {
    }
    Material::~Material()
    {
        for (uint32 paramidx = 0; paramidx < mParameters.GetSize(); paramidx++) {
            if (mParameters.GetAt(paramidx).value.GetType() == ShaderParameter::Type_Texture) {
                LE_ResourceManager.ReleaseResource((Texture*)(mParameters.GetAt(paramidx).value));
            }
			mParameters.GetAt(paramidx).value.Release();
        }
        mParameters.Clear();
    }
    void Material::Load(TextParser::NODE *root)
    {
        // TODO : Share shader between materials...
        if (mShader) {
            delete mShader;
        }
        mShader = new Shader();
		bool SucceedCompilingVS = false;
		bool SucceedCompilingPS = false;
		for (uint32 chidx = 0; chidx < root->children.size(); chidx++) {
            const TextParser::NODE *node = root->children[chidx];
            if (node->name == "ID") {
                mId = node->values[0];
            }
            else if (node->name == "NAME") {
                mName = node->values[0];
            }
            else if (node->name == "VERTEXSHADER") {
				SucceedCompilingVS = mShader->Compile(node->values[0], Shader::TYPE_VERTEX);
            }
            else if (node->name == "PIXELSHADER") {
				SucceedCompilingPS = mShader->Compile(node->values[0], Shader::TYPE_PIXEL);
            }
            else { // Texture or Parameter
                if (node->values.count() == 1) {
                    if (node->IsValueNumber(0)) { // float
                        mParameters[node->name] = node->values[0].ToFloat();
                    }
                    else { // texture
                        const ResourceManager::RESOURCE *resource = LE_ResourceManager.GetResourceWithRegister(node->values[0]);
                        if (resource)
                            mParameters[node->name] = static_cast<Texture*>(resource->data);
                    }
                }
                else if (node->values.count() == 2) { // fVector3
                    mParameters[node->name] = node->ToFloatVector2();
                }
                else if (node->values.count() == 3) { // fVector3
                    mParameters[node->name] = node->ToFloatVector3();
                }
                else if (node->values.count() == 4) { // fVector4
                    mParameters[node->name] = node->ToFloatVector4();
                }
                else if (node->values.count() == 16) { // fMatrix4x4
                    mParameters[node->name] = node->ToFloatMatrix4x4();
                }
            }
        }
		if (SucceedCompilingVS && SucceedCompilingPS) {
			LE_ShaderManager.AddShader(mShader);
		}
		else {
			delete mShader;
			mShader = NULL;
		}
    }
    void Material::setupShaderParameters()
    {
        if (mShader == NULL)
            return;
        if (mShaderDriverParameter = dynamic_cast<ShaderDriverParameter*>(mShader->GetDriver("ShaderDriverParameter"))) {
            for (uint32 prmidx = 0; prmidx < mShaderDriverParameter->GetParameterCount(); prmidx++) {
                ShaderDriverParameter::ShaderParameter &param = mShaderDriverParameter->GetParameter(prmidx);
                int indexOfMaterialParameters = mParameters.FindIndex(param.name);
                if (indexOfMaterialParameters >= 0) {
                    mParameters.GetAt(indexOfMaterialParameters).value.SetIndexOfShaderParameter(prmidx);
                }
            }
        }
    }
    void Material::SetupShaderParameters()
    {
        AutoPointer<RendererTask> rt_MaterialSetupShaderParameter = new RendererTask_MaterialSetupShaderParameters(this);
        LE_DrawManager.AddRendererTask(rt_MaterialSetupShaderParameter);
    }
    void Material::Bind(const RenderState &rs)
    {
        if (mShader) {
            DrawCommand::BindShader(mShader);

            for (uint32 prmidx = 0; prmidx < mParameters.size(); prmidx++) {
                if (mParameters.GetAt(prmidx).value.IndexOfShaderParameter() >= 0) {
                    ShaderDriverParameter::ShaderParameter &param = mShaderDriverParameter->GetParameter(mParameters.GetAt(prmidx).value.IndexOfShaderParameter());
                    switch (param.type) {
                        case ShaderDriverParameter::ShaderParameter::ParameterType_Texture:
                            param.data.texture = static_cast<Texture*>(mParameters.GetAt(prmidx).value);
                            break;
                        default:
                            break;
                    }
                }
            }

            mShader->ApplyDrivers(rs, this);
        }
    }
}