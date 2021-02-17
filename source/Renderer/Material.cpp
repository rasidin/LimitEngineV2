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
@author minseob (https://github.com/rasidin)
*********************************************************************/

#include "Renderer/Material.h"

#include "Core/MemoryAllocator.h"
#include "Factories/TextureFactory.h"
#include "Managers/DrawManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Shader.h"

#include "Shaders/Standard_prepass.vs.h"
#include "Shaders/Standard_prepass.ps.h"
#include "Shaders/Standard_basepass.vs.h"
#include "Shaders/Standard_basepass.ps.h"

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
        //if (InMaterial.mVertexShader[0].IsValid()) {
        //    String ShaderPrefix;
        //    String RenderPass;
        //    InMaterial.mShader[0]->GetName().Split(".", ShaderPrefix, RenderPass);
        //    shaderName = ShaderPrefix;
        //}
        //if (IsSaving() && !InMaterial.mShader[0].IsValid()) {
        //    *this << InMaterial.mName;
        //}
        //else 
        {
            *this << shaderName;
        }
        if (IsLoading()) {
            for (uint32 RenderPassIndex = 0; RenderPassIndex < (uint32)RenderPass::NumOfRenderPass; RenderPassIndex++) {
                InMaterial.mVertexShader[RenderPassIndex] = LE_ShaderManager.GetShader(shaderName + "_" + RenderPassNames[RenderPassIndex] + "_VS");
                InMaterial.mPixelShader[RenderPassIndex] = LE_ShaderManager.GetShader(shaderName + "_" + RenderPassNames[RenderPassIndex] + "_PS");
            }
        }
        return *this;
    }

    Material::Material()
        : mId()
        , mName()
    {
        ::memset(mIsEnabledRenderPass, 0, sizeof(mIsEnabledRenderPass));
        ::memset(mVSConstantUpdateBuffer, 0, sizeof(mVSConstantUpdateBuffer));
        ::memset(mPSConstantUpdateBuffer, 0, sizeof(mPSConstantUpdateBuffer));
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
            mVertexShader[Index] = nullptr;
            mPixelShader[Index] = nullptr;
            mVSConstantBuffer[Index] = nullptr;
            mPSConstantBuffer[Index] = nullptr;
            if (mVSConstantUpdateBuffer[Index]) {
                MemoryAllocator::Free(mVSConstantUpdateBuffer[Index]);
                mVSConstantUpdateBuffer[Index] = nullptr;
            }
            if (mPSConstantUpdateBuffer[Index]) {
                MemoryAllocator::Free(mPSConstantUpdateBuffer[Index]);
                mPSConstantUpdateBuffer[Index] = nullptr;
            }
        }
    }
    Material* Material::Load(TextParser::NODE *root)
    {
        // TODO : Share shader between materials...
        for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++) {
            mVertexShader[Index] = nullptr;
            mPixelShader[Index] = nullptr;
        }

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
                    if (ShaderManager::IsUsable()) {
                        mVertexShader[Index] = LE_ShaderManager.GetShader(shaderName + "_" + RenderPassNames[Index] + "_VS");
                        mPixelShader[Index] = LE_ShaderManager.GetShader(shaderName + "_" + RenderPassNames[Index] + "_PS");
                    }
                }
            }
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
        for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++) {
            mVertexShader[Index] = nullptr;
            mPixelShader[Index] = nullptr;
        }

        if (rapidxml::xml_node<const char> *IDNode = XMLNode->first_node("ID")) {
            mId = IDNode->value();
        }
        if (rapidxml::xml_node<const char> *ShaderNode = XMLNode->first_node("SHADER")) {
            String shaderName(ShaderNode->value());
            mName = shaderName;
            for (uint32 Index = 0; Index < (uint32)RenderPass::NumOfRenderPass; Index++) {
                if (ShaderManager::IsUsable()) {
                    mVertexShader[Index] = LE_ShaderManager.GetShader(shaderName + "_" + RenderPassNames[Index] + "_VS");
                    mPixelShader[Index] = LE_ShaderManager.GetShader(shaderName + "_" + RenderPassNames[Index] + "_PS");
                }
            }
        }

        return this;
    }
    bool Material::IsEnabledRenderPass(const RenderPass& InRenderPass) const
    {
        return mIsEnabledRenderPass[(uint32)InRenderPass] && mPixelShader[(uint32)InRenderPass].IsValid();
    }
    void Material::setupShaderParameters()
    {
        for (int rpidx = 0; rpidx < static_cast<int>(RenderPass::NumOfRenderPass); rpidx++) {
            if (mVertexShader[rpidx].IsValid()) {
                if (mVertexShader[rpidx]->GetBoundTextureCount())
                    mVSTexturePosition[rpidx].Setup(mVertexShader[rpidx]->GetBoundTextureNames(), mVertexShader[rpidx]->GetBoundTextureCount());
                if (mVertexShader[rpidx]->GetBoundSamplerCount())
                    mVSSamplerPosition[rpidx].Setup(mVertexShader[rpidx]->GetBoundSamplerNames(), mVertexShader[rpidx]->GetBoundSamplerCount());
                if (mVertexShader[rpidx]->GetShaderHash() == Standard_prepass_VS::GetHash()) {
                    mVSConstantBuffer[rpidx] = new ConstantBuffer();
                    mVSConstantBuffer[rpidx]->Create(sizeof(Standard_prepass_VS::ConstantBuffer0), nullptr);
                    mVSConstantUpdateBuffer[rpidx] = MemoryAllocator::Alloc(sizeof(Standard_prepass_VS::ConstantBuffer0));
                    mVSShaderDriver[rpidx].Setup(*(Standard_prepass_VS::ConstantBuffer0*)mVSConstantUpdateBuffer[rpidx]);
                }
                else if (mVertexShader[rpidx]->GetShaderHash() == Standard_basepass_VS::GetHash()) {
                    mVSConstantBuffer[rpidx] = new ConstantBuffer();
                    mVSConstantBuffer[rpidx]->Create(sizeof(Standard_basepass_VS::ConstantBuffer0), nullptr);
                    mVSConstantUpdateBuffer[rpidx] = MemoryAllocator::Alloc(sizeof(Standard_basepass_VS::ConstantBuffer0));
                    mVSShaderDriver[rpidx].Setup(*(Standard_basepass_VS::ConstantBuffer0*)mVSConstantUpdateBuffer[rpidx]);
                }
            }
            if (mPixelShader[rpidx].IsValid()) {
                if (mPixelShader[rpidx]->GetBoundTextureCount())
                    mPSTexturePosition[rpidx].Setup(mPixelShader[rpidx]->GetBoundTextureNames(), mPixelShader[rpidx]->GetBoundTextureCount());
                if (mPixelShader[rpidx]->GetBoundSamplerCount())
                    mPSSamplerPosition[rpidx].Setup(mPixelShader[rpidx]->GetBoundSamplerNames(), mPixelShader[rpidx]->GetBoundSamplerCount());
                if (mPixelShader[rpidx]->GetShaderHash() == Standard_basepass_PS::GetHash()) {
                    mPSConstantBuffer[rpidx] = new ConstantBuffer();
                    mPSConstantBuffer[rpidx]->Create(sizeof(Standard_basepass_PS::ConstantBuffer0), nullptr);
                    mPSConstantUpdateBuffer[rpidx] = MemoryAllocator::Alloc(sizeof(Standard_basepass_PS::ConstantBuffer0));
                    mPSShaderDriver[rpidx].Setup(*(Standard_basepass_PS::ConstantBuffer0*)mPSConstantUpdateBuffer[rpidx]);
                }
            }
        }
    }
    void Material::SetupShaderParameters()
    {
        if (DrawManager::IsUsable()) {
            AutoPointer<RendererTask> rt_MaterialSetupShaderParameter = new RendererTask_MaterialSetupShaderParameters(this);
            LE_DrawManager.AddRendererTask(rt_MaterialSetupShaderParameter);
        }
    }
    void Material::ReadyToRender(const RenderState& rs, PipelineStateDescriptor& desc)
    {
        uint32 renderPass = (uint32)rs.GetRenderPass();
        if (mVertexShader[renderPass].IsValid() && mPixelShader[renderPass].IsValid()) {
            desc.Shaders[static_cast<int>(Shader::Type::Vertex)] = mVertexShader[renderPass].Get();
            desc.Shaders[static_cast<int>(Shader::Type::Pixel)] = mPixelShader[renderPass].Get();

            uint32 cbidx = 0u;
            if (mVSConstantBuffer[renderPass].IsValid()) {
                rs.SetToShaderDriver(mVSShaderDriver[renderPass]);
                DrawCommand::UpdateConstantBuffer(mVSConstantBuffer[renderPass].Get(), mVSConstantUpdateBuffer[renderPass], mVSConstantBuffer[renderPass]->GetSize());
            }
            if (mPSConstantBuffer[renderPass].IsValid()) {
                rs.SetToShaderDriver(mPSShaderDriver[renderPass]);
                DrawCommand::UpdateConstantBuffer(mPSConstantBuffer[renderPass].Get(), mPSConstantUpdateBuffer[renderPass], mPSConstantBuffer[renderPass]->GetSize());
            }
        }
    }
    void Material::Bind(const RenderState &rs)
    {
        uint32 renderPass = (uint32)rs.GetRenderPass();
        uint32 cbidx = 0u;
        rs.SetTextures(mVSTexturePosition[renderPass]);
        rs.SetSamplers(mVSSamplerPosition[renderPass]);
        rs.SetTextures(mPSTexturePosition[renderPass]);
        rs.SetSamplers(mPSSamplerPosition[renderPass]);
        if (mVSConstantBuffer[renderPass].IsValid()) {
            DrawCommand::SetConstantBuffer(cbidx++, mVSConstantBuffer[renderPass].Get());
        }
        if (mPSConstantBuffer[renderPass].IsValid()) {
            DrawCommand::SetConstantBuffer(cbidx++, mPSConstantBuffer[renderPass].Get());
        }
    }
}