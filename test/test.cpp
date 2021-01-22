#include <TBApplication.h>
#include <TBWidget.h>

#include <LEIntVector2.h>

#include <LimitEngine.h>
#include <Core/SerializableResource.h>
#include <Renderer/CinemaCamera.h>
#include <Factories/ArchiveFactory.h>
#include <Factories/TextureFactory.h>
#include <Factories/ModelFactory.h>
#include <Managers/ResourceManager.h>
#include <Managers/ShaderManager.h>
#include <Renderer/Material.h>
#include <Renderer/Model.h>
#include <Renderer/Texture.h>
#include <Renderer/LightIBL.h>
#include <Renderer/Transform.h>

#include <iostream>

class LimitEngineWindow : public ToolBase::TBWidget
{
public:
	LimitEngineWindow(const char *Title, LimitEngine::LimitEngine *InEngine) 
        : ToolBase::TBWidget(Title, 1920, 1080) 
        , Engine(InEngine)
        , mBackgroundImage(nullptr)
        , mSphereModel(nullptr)
    {}
	
    ~LimitEngineWindow()
    {
        mCamera = nullptr;
        mBackgroundImage = nullptr;
        mSphereModel = nullptr;
        mPlaneModel = nullptr;
        mCarModel = nullptr;
    }

    void SetupScene() {
        mBackgroundImage = Engine->LoadTexture("textures/MyCarlots.Background.lea", LimitEngine::ArchiveFactory::ID, false);

        if (mBackgroundImage.IsValid()) {
            mBackgroundImage->InitResource();
            Engine->SetBackgroundImage(mBackgroundImage, LimitEngine::BackgroundImageType::Fullscreen);
        }
#if 0 // Temporaily
#if 0 // 19F test
        const float CameraHeight = 1.2f;
        const float TableHeight = 1.05f;
        const float BallRadius = 0.1f;
        const float BallHeight = 0.5f;

        mCamera = new LimitEngine::CinemaCamera();
        mCamera->SetFocalLength(30.0f * 18.0f / 16.0f);
        mCamera->SetPosition( LEMath::FloatVector3(0.0f, CameraHeight,-2.85f));
        mCamera->SetDirection(LEMath::FloatVector3(0.0f, 0.0f, 1.0f));
        mCamera->SetShutterSpeed(1.0f / 8.0f);
        mCamera->SetFStop(1.0f / 4.0f);
        mCamera->SetExposureOffset(-7.0f);
        Engine->SetMainCamera(mCamera.Get());

        {
            LimitEngine::LightIBL *DefaultIBL = new LimitEngine::LightIBL();

            if (LimitEngine::Texture *IBLReflectionTexture = Engine->LoadTexture("textures/19F_Reflection_2K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLReflectionTexture->InitResource();
                DefaultIBL->SetIBLReflectionTexture(IBLReflectionTexture);
            }
            if (LimitEngine::Texture *IBLIrradianceTexture = Engine->LoadTexture("textures/MyCarlots.Irradiance.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLIrradianceTexture->InitResource();
                DefaultIBL->SetIBLIrradianceTexture(IBLIrradianceTexture);
            }
            Engine->AddLight(DefaultIBL);
        }

        mPlaneModel = Engine->LoadModel("models/plane.model.text", LimitEngine::ModelFactory::ID, false);
        if (mPlaneModel.IsValid())
        {
            mPlaneModel->InitResource();
            mPlaneModelInstanceID = Engine->AddModel(mPlaneModel);
            Engine->UpdateModelTransform(
                mPlaneModelInstanceID,
                LimitEngine::Transform(
                    LEMath::FloatVector4(0.0f, TableHeight, 0.0f, 1.0f),
                    LEMath::FloatVector4::Zero,
                    LEMath::FloatVector4(0.5f, 0.5f, 0.5f, 1.0f)
                ));
        }

        //mSphereModel = Engine->LoadModel("models/sphere.model.text", LimitEngine::ModelFactory::ID, false);

        mSphereModel = Engine->LoadModel("models/sphere.model.lea", LimitEngine::ArchiveFactory::ID, false);
        if (mSphereModel.IsValid()) 
        {
            mSphereModel->InitResource();
            mSphereModelInstanceID = Engine->AddModel(mSphereModel);
            Engine->UpdateModelTransform(
                mSphereModelInstanceID,
                LimitEngine::Transform(
                    LEMath::FloatVector4(0.0f, TableHeight + BallHeight, 0.0f, 0.0f),
                    LEMath::FloatVector4::Zero,
                    LEMath::FloatVector4(BallRadius, BallRadius, BallRadius, 0.0f)
                ));
        }

        mCarModel = Engine->LoadModel("models/Porsche.model.lea", LimitEngine::ArchiveFactory::ID, false);
        //mCarModel = Engine->LoadModel("models/Porsche.model.xml", LimitEngine::ModelFactory::ID, false);
        if (mCarModel.IsValid())
        {
            mCarModel->InitResource();
            mCarModelInstanceID = Engine->AddModel(mCarModel);
            Engine->UpdateModelTransform(
                mCarModelInstanceID,
                LimitEngine::Transform(
                    LEMath::FloatVector4(0.0f, TableHeight, 0.0f, 0.0f),
                    LEMath::FloatVector4(0.0f, 0.5f * LEMath::LEMath_PI, 0.0f, 0.0f),
                    LEMath::FloatVector4(0.003f, 0.003f, 0.003f, 1.0f)
                ));
        }
        
        //if (mBackgroundImage = Engine->LoadTexture("textures/19F_reflection_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
        //if (mBackgroundImage = Engine->LoadTexture("textures/19F_Irradiance_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
        //mBackgroundImage = Engine->LoadTexture("textures/19F_Background_2K.texture.lea", LimitEngine::ArchiveFactory::ID, false);
        mBackgroundImage = Engine->LoadTexture("textures/MyCarlots.Background.lea", LimitEngine::ArchiveFactory::ID, false);

        //if (LimitEngine::TextureFactory *Factory = (LimitEngine::TextureFactory*)LimitEngine::ResourceManager::GetSingleton().GetFactory(LimitEngine::TextureFactory::ID)) {
        //    Factory->SetImportFilter(LimitEngine::TextureFactory::TextureImportFilter::Irradiance);
        //    Factory->SetSizeFilteredImage(LEMath::IntVector2(128, 64));
        //    Factory->SetIrrdianceSampleCount(1024);
        //}
        //mBackgroundImage = Engine->LoadTexture("textures/Alexs_Apt_2k.tga", LimitEngine::TextureFactory::ID, false);

        if (mBackgroundImage.IsValid()) {
            mBackgroundImage->InitResource();
            Engine->SetBackgroundImage(mBackgroundImage, LimitEngine::BackgroundImageType::Fullscreen);
        }
#else
        const float CameraHeight = 1.34f;
        const float TableHeight = 0.0f;
        const float BallRadius = 0.25f;
        const float BallHeight = 2.0f;

        mBackgroundExposure = 0.0f;
        mBackgroundXYZScale = LEMath::FloatVector3(0.91f, 0.89f, 0.47f);
        Engine->SetBackgroundXYZScale(mBackgroundXYZScale);
        Engine->SetBackgroundExposure(mBackgroundExposure);

        mCarPosition = LEMath::FloatVector4(0.0f, 0.0f,-0.09f, 0.0f);
        mCarRotation = LEMath::FloatVector4(0.0f, 0.0f, 0.0f, 0.0f);
        mCarScale = LEMath::FloatVector4(0.012f, 0.012f, 0.012f, 1.0f);

        mCamera = new LimitEngine::CinemaCamera();
        mCamera->SetCloppedScale(LEMath::FloatSize(16.0f / 18.0f, 1.0f / 4.0f));
        mCamera->SetFocalLength(35.0f);
        mCamera->SetPosition(LEMath::FloatVector3(0.0f, CameraHeight, 8.6f));
        mCamera->SetDirection(LEMath::FloatVector3(0.0f, 0.0f,-1.0f));
        mCamera->SetShutterSpeed(1.0f / 8.0f);
        mCamera->SetFStop(1.0f / 4.0f);
        mCamera->SetExposureOffset(-7.0f);
        Engine->SetMainCamera(mCamera.Get());

        {
            LimitEngine::LightIBL *DefaultIBL = new LimitEngine::LightIBL();

            //if (LimitEngine::Texture *IBLReflectionTexture = Engine->LoadTexture("textures/19F_Reflection_2K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
            if (LimitEngine::Texture *IBLReflectionTexture = Engine->LoadTexture("textures/MyCarlots.Reflection.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLReflectionTexture->InitResource();
                DefaultIBL->SetIBLReflectionTexture(IBLReflectionTexture);
            }
            if (LimitEngine::Texture *IBLIrradianceTexture = Engine->LoadTexture("textures/MyCarlots.Irradiance.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLIrradianceTexture->InitResource();
                DefaultIBL->SetIBLIrradianceTexture(IBLIrradianceTexture);
            }
            Engine->AddLight(DefaultIBL);
        }

        mPlaneModel = Engine->LoadModel("models/plane.model.text", LimitEngine::ModelFactory::ID, false);
        if (mPlaneModel.IsValid())
        {
            if (LimitEngine::Material *material = mPlaneModel->GetMaterial(0)) {
                material->SetShader(LimitEngine::RenderPass::PrePass, LimitEngine::ShaderManager::GetSingleton().GetShader("OcclusionOnly.PrePass").Get());
                material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("OcclusionOnly.BasePass").Get());
            }

            mPlaneModel->InitResource();
            mPlaneModelInstanceID = Engine->AddModel(mPlaneModel);
            Engine->UpdateModelTransform(
                mPlaneModelInstanceID,
                LimitEngine::Transform(
                    LEMath::FloatVector4(0.0f, TableHeight, 0.0f, 1.0f),
                    LEMath::FloatVector4::Zero,
                    LEMath::FloatVector4(4.0f, 1.0f, 5.7f * 0.5f, 1.0f) // Big size
                    //LEMath::FloatVector4(4.19f * 0.5f, 1.0f, 5.7f * 0.5f, 1.0f) // Floor size
                    //LEMath::FloatVector4(1.88f * 0.5f, 1.0f, 4.557f * 0.5f, 1.0f) // Car size
                ));
        }
/*
        mSphereModel = Engine->LoadModel("models/sphere.model.lea", LimitEngine::ArchiveFactory::ID, false);
        if (mSphereModel.IsValid())
        {
            if (LimitEngine::Material *material = mSphereModel->GetMaterial(0)) {
                material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                LimitEngine::ShaderParameter BaseColorShaderParameter;
                BaseColorShaderParameter = LEMath::FloatVector3(1.0f, 1.0f, 1.0f);
                material->SetParameter("BaseColor", BaseColorShaderParameter);

                LimitEngine::ShaderParameter MetallicShaderParameter;
                MetallicShaderParameter = 1.0f;
                material->SetParameter("Metallic", MetallicShaderParameter);

                LimitEngine::ShaderParameter RoughnessShaderParameter;
                RoughnessShaderParameter = 0.0f;
                material->SetParameter("Roughness", RoughnessShaderParameter);
            }

            mSphereModel->InitResource();
            mSphereModelInstanceID = Engine->AddModel(mSphereModel);
            Engine->UpdateModelTransform(
                mSphereModelInstanceID,
                LimitEngine::Transform(
                    LEMath::FloatVector4(0.0f, BallHeight, 0.0f, 0.0f),
                    LEMath::FloatVector4::Zero,
                    LEMath::FloatVector4(BallRadius, BallRadius, BallRadius, 0.0f)
                ));
        }
*/
        //mCarModel = Engine->LoadModel("models/Porsche.model.xml", LimitEngine::ModelFactory::ID, false);
        mCarModel = Engine->LoadModel("models/Porsche.model.lea", LimitEngine::ArchiveFactory::ID, false);
        if (mCarModel.IsValid())
        {
            // Override materials
            for (uint32 matIndex = 0; matIndex < mCarModel->GetMaterialCount(); matIndex++) {
                if (LimitEngine::Material *material = mCarModel->GetMaterial(matIndex)) {
                    if (material->GetID().IsContain("chrome")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(1.0f, 1.0f, 1.0f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 1.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.05f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("interior")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.05f, 0.05f, 0.05f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.01f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.4f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("gum")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.0f, 0.0f, 0.0f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.05f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.4f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("silver")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(1.0f, 1.0f, 1.0f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 1.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.5f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("glass")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());
                        material->SetShader(LimitEngine::RenderPass::TranslucencyPass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.TranslucencyPass").Get());

                        material->SetEnabledRenderPass(LimitEngine::RenderPass::PrePass, false);
                        material->SetEnabledRenderPass(LimitEngine::RenderPass::BasePass, false);
                        material->SetEnabledRenderPass(LimitEngine::RenderPass::TranslucencyPass, true);

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector4(0.0f, 0.0f, 0.0f, 0.5f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.8f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.05f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("body")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.18f, 0.18f, 0.18f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.04f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.05f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("tire")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.0f, 0.0f, 0.0f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.1f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("black")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.01f, 0.01f, 0.01f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.02f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.05f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("dark_grey")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.0f, 0.0f, 0.0f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.05f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.02f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("red")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.9f, 0.05f, 0.05f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.3f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("yellow")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.9f, 0.9f, 0.05f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.3f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("orange")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(0.9f, 0.5f, 0.05f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.3f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else if (material->GetID().IsContain("logo") ||
                             material->GetID().IsContain("carbon") ||
                             material->GetID().IsContain("navi") ||
                             material->GetID().IsContain("clock")) {
                        material->SetShader(LimitEngine::RenderPass::BasePass, LimitEngine::ShaderManager::GetSingleton().GetShader("StandardParamOnly.BasePass").Get());

                        LimitEngine::ShaderParameter BaseColorShaderParameter;
                        BaseColorShaderParameter = LEMath::FloatVector3(1.0f, 0.5f, 1.0f);
                        material->SetParameter("BaseColor", BaseColorShaderParameter);
                        LimitEngine::ShaderParameter MetallicShaderParameter;
                        MetallicShaderParameter = 0.0f;
                        material->SetParameter("Metallic", MetallicShaderParameter);
                        LimitEngine::ShaderParameter RoughnessShaderParameter;
                        RoughnessShaderParameter = 0.01f;
                        material->SetParameter("Roughness", RoughnessShaderParameter);
                    }
                    else {
                        material->SetEnabledRenderPass(LimitEngine::RenderPass::PrePass, false);
                        material->SetEnabledRenderPass(LimitEngine::RenderPass::BasePass, false);
                        material->SetEnabledRenderPass(LimitEngine::RenderPass::TranslucencyPass, false);
                    }
                }
            }

            mCarModel->InitResource();
            mCarModelInstanceID = Engine->AddModel(mCarModel);
            Engine->UpdateModelTransform(
                mCarModelInstanceID,
                LimitEngine::Transform(
                    mCarPosition,
                    mCarRotation,
                    mCarScale
                ));
        }

        mBackgroundImage = Engine->LoadTexture("textures/MyCarlots.Background.lea", LimitEngine::ArchiveFactory::ID, false);

        if (mBackgroundImage.IsValid()) {
            mBackgroundImage->InitResource();
            Engine->SetBackgroundImage(mBackgroundImage, LimitEngine::BackgroundImageType::Fullscreen);
        }
#endif
#endif
    }

	virtual void FrameUpdate(double frameTime) override {
        Engine->Update();
	}

    virtual void KeyboardEvent(const ToolBase::KeyboardEvent &keyEvent) override {
        if (keyEvent.status == ToolBase::KeyboardEvent::STATUS_DOWN) {
            switch (keyEvent.key)
            {
            case 0x31: // 1
                mBackgroundExposure += 0.01f;
                break;
            case 0x32: // 2
                mBackgroundExposure -= 0.01f;
                break;
            case 0x33: // 3
                mBackgroundXYZScale.SetX(mBackgroundXYZScale.X() + 0.01f);
                break;
            case 0x34: // 4
                mBackgroundXYZScale.SetX(mBackgroundXYZScale.X() - 0.01f);
                break;
            case 0x35: // 5
                mBackgroundXYZScale.SetY(mBackgroundXYZScale.Y() + 0.01f);
                break;
            case 0x36: // 6
                mBackgroundXYZScale.SetY(mBackgroundXYZScale.Y() - 0.01f);
                break;
            case 0x37: // 7
                mBackgroundXYZScale.SetZ(mBackgroundXYZScale.Z() + 0.01f);
                break;
            case 0x38: // 8
                mBackgroundXYZScale.SetZ(mBackgroundXYZScale.Z() - 0.01f);
                break;
            case 0x26: // UP
                mCarPosition.SetZ(mCarPosition.Z() + 0.01f);
                break;
            case 0x28: // DOWN
                mCarPosition.SetZ(mCarPosition.Z() - 0.01f);
                break;
            case 0x25: // LEFT
                mCarPosition.SetX(mCarPosition.X() - 0.01f);
                break;
            case 0x27: // RIGHT
                mCarPosition.SetX(mCarPosition.X() + 0.01f);
                break;
            case 0x50: // p
                mCarScale.SetX(mCarScale.X() + 0.0001f);
                mCarScale.SetY(mCarScale.Y() + 0.0001f);
                mCarScale.SetZ(mCarScale.Z() + 0.0001f);
                break;
            case 0xbc: // ,
                mCarScale.SetX(mCarScale.X() - 0.0001f);
                mCarScale.SetY(mCarScale.Y() - 0.0001f);
                mCarScale.SetZ(mCarScale.Z() - 0.0001f);
                break;
            case 0x57: // W
                mCamera.Get()->SetPosition(mCamera.Get()->GetPosition() + LEMath::FloatVector3(0.0f, 0.0f, 0.01f));
                break;
            case 0x58: // X
                mCamera.Get()->SetPosition(mCamera.Get()->GetPosition() - LEMath::FloatVector3(0.0f, 0.0f, 0.01f));
                break;
            case 0x51: // Q
                mCamera.Get()->SetPosition(mCamera.Get()->GetPosition() + LEMath::FloatVector3(0.0f, 0.01f, 0.0f));
                break;
            case 0x45: // E
                mCamera.Get()->SetPosition(mCamera.Get()->GetPosition() - LEMath::FloatVector3(0.0f, 0.01f, 0.0f));
                break;
            case 0x5A: // Z
                mCamera->SetFocalLength(mCamera->GetFocalLength() + 0.01f);
                break;;
            case 0x43: // C
                mCamera->SetFocalLength(mCamera->GetFocalLength() - 0.01f);
                break;
            }
            Engine->UpdateModelTransform(
                mCarModelInstanceID,
                LimitEngine::Transform(
                    mCarPosition,
                    mCarRotation,
                    mCarScale
                ));
            Engine->SetBackgroundXYZScale(mBackgroundXYZScale);
            Engine->SetBackgroundExposure(mBackgroundExposure);
        }
    }
private:
    LimitEngine::LimitEngine    *Engine;

    LEMath::FloatVector3 mBackgroundXYZScale;
    float                mBackgroundExposure;

    LEMath::FloatVector4 mCarPosition;
    LEMath::FloatVector4 mCarRotation;
    LEMath::FloatVector4 mCarScale;

    LimitEngine::ReferenceCountedPointer<LimitEngine::CinemaCamera>   mCamera;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Texture>        mBackgroundImage;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>          mSphereModel;
    uint32                                                            mSphereModelInstanceID;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>          mPlaneModel;
    uint32                                                            mPlaneModelInstanceID;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>          mCarModel;
    uint32                                                            mCarModelInstanceID;
};

int main()
{    
    static constexpr size_t TotalMemory = 1024 << 20; // 1 GiB

	LimitEngine::MemoryAllocator::Init();
	LimitEngine::MemoryAllocator::InitWithMemoryPool(TotalMemory);
    LimitEngine::LimitEngine *Engine = new LimitEngine::LimitEngine();
    Engine->SetResourceRootPath("../../../../resources");

    LimitEngine::InitializeOptions engineOptions(
        LEMath::IntSize(1920, 1080),
        LimitEngine::InitializeOptions::ColorSpace::Linear,
        LimitEngine::InitializeOptions::ColorSpace::sRGB
    );

	ToolBase::TBApplication app;
	LimitEngineWindow window("Test", Engine);
    Engine->Init(window.GetHandle(), engineOptions);
    window.SetupScene();
	app.Run();
    Engine->Term();
	window.~LimitEngineWindow();

	delete Engine;
	LimitEngine::MemoryAllocator::Term();

    return 0;
}