/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ShaderDriverBakedBRDF.cpp
@brief ShaderDriver for baked brdf lighting
@author minseob (leeminseob@outlook.com)
***********************************************************/

#include "Renderer/ShaderDriverBakedBRDF.h"

#include "Managers/ShaderManager.h"
//#include "Managers/LightManager.h"
#include "Renderer/DrawCommand.h"
#include "Managers/DrawManager.h"
#include "Renderer/Texture.h"
#include "Renderer/LightIBL.h"
#include "Renderer/Material.h"

namespace LimitEngine {
	class RendererTask_GenerateBakedBRDFTexture : public RendererTask
	{
	public:
		RendererTask_GenerateBakedBRDFTexture(ShaderDriverBakedBRDF *target) 
			: mTarget(target)
		{}
		void Run() override
		{
			//if (mTarget->mBakedBRDFTexture) {
			//	delete mTarget->mBakedBRDFTexture;
			//	mTarget->mBakedBRDFTexture = NULL;
			//}
			////if (mTarget->mBRDFLUTTexture == NULL)
			////	return;

			//Shader *ComputeShader = LE_ShaderManager.GetShader("GenerateBakedBRDF");
			//DrawCommand::BindShader(ComputeShader);

			//int brdfLUTTexture_Position = ComputeShader->GetTextureLocation("BRDFLUTTexture");
			//int iblTexture_Position = ComputeShader->GetTextureLocation("IBLTexture");
			//int outBakedBRDFTexture_Position = ComputeShader->GetTextureLocation("OutBakedBRDFTexture");

			//LightIBL *iblLight = LE_LightManager.GetStandardIBLLight();
			//Texture *iblSpecularTexture = iblLight->GetIBLSpecularTexture();

			//if (iblSpecularTexture == NULL)
			//	return;

			//iSize textureSize = iSize(1024, 512);
			//mTarget->mBakedBRDFTexture = new Texture();
			//mTarget->mBakedBRDFTexture->Create(textureSize, TEXTURE_COLOR_FORMAT_A8R8G8B8);
			//
			//if (brdfLUTTexture_Position >= 0) {
			//	DrawCommand::BindTexture(brdfLUTTexture_Position, mTarget->mBRDFLUTTexture);
			//}
			//if (iblTexture_Position >= 0) {
			//	DrawCommand::BindTexture(iblTexture_Position, iblSpecularTexture);
			//}
			//if (outBakedBRDFTexture_Position >= 0) {
			//	DrawCommand::BindTargetTexture(outBakedBRDFTexture_Position, mTarget->mBakedBRDFTexture);
			//}

			//iSize gridSize = textureSize / 32;
			//DrawCommand::Dispatch(gridSize.x, gridSize.y, 1);
			//DrawCommand::BindTargetTexture(outBakedBRDFTexture_Position, NULL);
		}
		static bool IsReadyToGenerate(ShaderDriverBakedBRDF *target)
		{
			return target->mBRDFLUTTexture != NULL;
		}
	private:
		ShaderDriverBakedBRDF *mTarget;
	};
	ShaderDriverBakedBRDF::~ShaderDriverBakedBRDF()
	{
		if (mBakedBRDFTexture) {
			delete mBakedBRDFTexture;
			mBakedBRDFTexture = NULL;
		}
	}
	bool ShaderDriverBakedBRDF::IsValid(const ShaderParameterParser::ParameterMap &paramMap) const
	{
		if (paramMap.FindIndex("BakedBRDFTexture") >= 0)
			return true;
		return false;
	}
	void ShaderDriverBakedBRDF::Apply(const RenderState &rs, const Material *material)
	{
		if (mBRDFLUTTexture == NULL) {
			mBRDFLUTTexture = (Texture*)material->GetParameter("BRDFLUTTexture");
		}
		if (mBakedBRDFTexture == NULL) {
			if (RendererTask_GenerateBakedBRDFTexture::IsReadyToGenerate(this)) {
				AutoPointer<RendererTask> rt_bakedBRDF = new RendererTask_GenerateBakedBRDFTexture(this);
				LE_DrawManager.AddRendererTask(rt_bakedBRDF);
			}
			return;
		}
		if (IsValidShaderParameterPosition(mBakedBRDF_Position)) {
			DrawCommand::BindTexture(mBakedBRDF_Position, mBakedBRDFTexture);
		}
		if (IsValidShaderParameterPosition(mBRDFLUT_Position)) {
			DrawCommand::BindTexture(mBRDFLUT_Position, mBRDFLUTTexture);
		}
	}
	void ShaderDriverBakedBRDF::setup(const ShaderParameterParser::ParameterMap &paramMap)
	{
		mBakedBRDF_Position = getShaderTextureLocation(paramMap, "BakedBRDFTexture");
		mBRDFLUT_Position = getShaderTextureLocation(paramMap, "BRDFLUTTexture");
	}
}