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
@file  Material.cpp
@brief Material Class
@author minseob (leeminseob@outlook.com)
*********************************************************************/

#include "Renderer/Material.h"

#include "Factories/TextureFactory.h"
#include "Managers/DrawManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/DrawCommand.h"
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

    template<> Archive& Archive::operator << (Material &InMaterial) {
        *this << InMaterial.mId;
        *this << InMaterial.mName;
        String shaderName;
        if (InMaterial.mShader[0].IsValid()) {
            String ShaderPrefix;
            String RenderPass;
            InMaterial.mShader[0]->GetName().Split(".", ShaderPrefix, RenderPass);
            shaderName = ShaderPrefix;
        }
        if (IsSaving() && !InMaterial.mShader[0].IsValid()) {
            *this << InMaterial.mName;
        }
        else {
            *this << shaderName;
        }
        if (IsLoading()) {
            for (uint32 RenderPassIndex = 0; RenderPassIndex < (uint32)RenderPass::NumOfRenderPass; RenderPassIndex++) {
                InMaterial.mShader[RenderPassIndex] = LE_ShaderManager.GetShader(shaderName + "." + RenderPassNames[RenderPassIndex]);
            }
        }
        return *this;
    }

    Material::Material()
        : mId()
        , mName()
    {
        ::memset(mShaderDriverParameter, 0, sizeof(ShaderDriverParameter*) * (uint32)RenderPass::NumOfRenderPass);
        ::memset(mIsEnabledRenderPass, 0, sizeof(mIsEnabledRenderPass));
        mIsEnabledRenderPass[(uint32)RenderPass::PrePass] = true;
        mIsEnabledRenderPass[(uint32)RenderPass::BasePass] = true;
    }
    Material::~Material()
    {
        for (uint32 paramidx = 0; paramidx < mParameters.GetSize(); paramidx++) {
			mParameters.GetAt(paramidx).value.Release();
        }
        mParameters.Clear();
        for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++) {
            mShader[Index] = nullptr;
            mConstantBuffer[Index] = nullptr;
        }
    }
    Material* Material::Load(TextParser::NODE *root)
    {
        // TODO : Share shader between materials...
        for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++)
            mShader[Index] = nullptr;

        //bool CompiledShaders = false;
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
            else if (node->name == "SHADER") {
                String shaderName = node->values[0];
                for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++) {
                    if (ShaderManager::IsUsable())
                        mShader[Index] = LE_ShaderManager.GetShader(shaderName + "." + RenderPassNames[Index]);
                }
            }
    //        else if (node->name == "VERTEXSHADER") {
    //            if (!mShader.IsValid()) mShader = new Shader();
    //            CompiledShaders = true;
				//SucceedCompilingVS = mShader->Compile(node->values[0], Shader::TYPE_VERTEX);
    //        }
    //        else if (node->name == "PIXELSHADER") {
    //            if (!mShader.IsValid()) mShader = new Shader();
    //            CompiledShaders = true;
    //            SucceedCompilingPS = mShader->Compile(node->values[0], Shader::TYPE_PIXEL);
    //        }
            else { // Texture or Parameter
                if (node->values.count() == 1) {
                    if (node->IsValueNumber(0)) { // float
                        mParameters[node->name] = node->values[0].ToFloat();
                    }
                    else { // texture
                        const ResourceManager::RESOURCE *resource = LE_ResourceManager.GetResourceWithRegister(node->values[0], TextureFactory::ID);
                        if (resource)
                            mParameters[node->name] = reinterpret_cast<Texture*>(resource->data);
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
		//if (CompiledShaders && SucceedCompilingVS && SucceedCompilingPS) {
		//	LE_ShaderManager.AddShader(mShader.Get());
		//}
		//else if (CompiledShaders) {
		//	mShader = nullptr;
		//}

        return this;
    }
    Material* Material::Load(rapidxml::xml_node<const char> *XMLNode)
    {
        for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++)
            mShader[Index] = nullptr;

        if (rapidxml::xml_node<const char> *IDNode = XMLNode->first_node("ID")) {
            mId = IDNode->value();
        }
        if (rapidxml::xml_node<const char> *ShaderNode = XMLNode->first_node("SHADER")) {
            String shaderName(ShaderNode->value());
            mName = shaderName;
            for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++) {
                if (ShaderManager::IsUsable())
                    mShader[Index] = LE_ShaderManager.GetShader(shaderName + "." + RenderPassNames[Index]);
            }
        }

        return this;
    }
    bool Material::IsEnabledRenderPass(const RenderPass& InRenderPass) const
    {
        return mIsEnabledRenderPass[(uint32)InRenderPass] && mShader[(uint32)InRenderPass].IsValid();
    }
    void Material::setupShaderParameters()
    {
        for (uint32 renderPassIndex = 0; renderPassIndex < (uint32)RenderPass::NumOfRenderPass; renderPassIndex++) {
            if (!mShader[renderPassIndex].IsValid())
                continue;

            for (uint32 paramIndex = 0; paramIndex < mParameters.size(); paramIndex++) {
                mParameters.GetAt(paramIndex).value.SetIndexOfShaderParameter(
                    renderPassIndex, mShader[renderPassIndex]->GetUniformLocation(mParameters.GetAt(paramIndex).key));
            }

            mConstantBuffer[renderPassIndex] = new ConstantBuffer();
            mConstantBuffer[renderPassIndex]->Create(mShader[renderPassIndex].Get());
        }
    }
    void Material::SetupShaderParameters()
    {
        if (DrawManager::IsUsable()) {
            AutoPointer<RendererTask> rt_MaterialSetupShaderParameter = new RendererTask_MaterialSetupShaderParameters(this);
            LE_DrawManager.AddRendererTask(rt_MaterialSetupShaderParameter);
        }
    }
    void Material::Bind(const RenderState &rs)
    {
        uint32 renderPass = (uint32)rs.GetRenderPass();
        if (mShader[renderPass].IsValid()) {
            DrawCommand::BindShader(mShader[renderPass].Get());
            DrawCommand::BindConstantBuffer(mConstantBuffer[renderPass].Get());

            for (uint32 prmidx = 0; prmidx < mParameters.size(); prmidx++) {
                int indexOfShaderParameter = mParameters.GetAt(prmidx).value.IndexOfShaderParameter(renderPass);
                if (indexOfShaderParameter >= 0) {
                    ShaderParameter &shaderParameter = mParameters.GetAt(prmidx).value;
                    switch (shaderParameter.GetType())
                    {
                    case ShaderParameter::Type_Float:
                        mShader[renderPass]->SetUniformFloat1(mConstantBuffer[renderPass].Get(), indexOfShaderParameter, shaderParameter);
                        break;
                    case ShaderParameter::Type_Float2:
                        mShader[renderPass]->SetUniformFloat2(mConstantBuffer[renderPass].Get(), indexOfShaderParameter, shaderParameter);
                        break;
                    case ShaderParameter::Type_Float3:
                        mShader[renderPass]->SetUniformFloat3(mConstantBuffer[renderPass].Get(), indexOfShaderParameter, shaderParameter);
                        break;
                    case ShaderParameter::Type_Float4:
                        mShader[renderPass]->SetUniformFloat4(mConstantBuffer[renderPass].Get(), indexOfShaderParameter, shaderParameter);
                        break;
                    default:
                        break;
                    }
                }
            }

            mShader[renderPass]->ApplyDrivers(rs, this);
        }
    }
}