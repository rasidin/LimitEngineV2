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
    }

    void SetupScene() {
        const float CameraHeight = 1.2f;
        const float TableHeight = 1.05f;
        const float BallRadius = 0.25f;

        mCamera = new LimitEngine::CinemaCamera();
        mCamera->SetFocalLength(30.0f * 18.0f / 16.0f);
        mCamera->SetPosition( LEMath::FloatVector3(0.0f, CameraHeight,-2.85f));
        mCamera->SetDirection(LEMath::FloatVector3(0.0f, 0.0f, 1.0f));
        mCamera->SetShutterSpeed(1.0f / 8.0f);
        mCamera->SetFStop(1.0f / 4.0f);
        mCamera->SetExposureOffset(-5.0f);
        Engine->SetMainCamera(mCamera.Get());

        {
            LimitEngine::LightIBL *DefaultIBL = new LimitEngine::LightIBL();

            //if (LimitEngine::Texture *IBLReflectionTexture = Engine->LoadTexture("textures/Alexs_Apt_2k.tga", LimitEngine::TextureFactory::ID, false)) {
            if (LimitEngine::Texture *IBLReflectionTexture = Engine->LoadTexture("textures/19F_Reflection_2K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLReflectionTexture->InitResource();
                DefaultIBL->SetIBLReflectionTexture(IBLReflectionTexture);
            }
            if (LimitEngine::Texture *IBLIrradianceTexture = Engine->LoadTexture("textures/19F_Irradiance.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
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
                    LEMath::FloatVector4(0.0f, TableHeight + BallRadius, 0.0f, 0.0f),
                    LEMath::FloatVector4::Zero,
                    LEMath::FloatVector4(BallRadius, BallRadius, BallRadius, 0.0f)
                ));
        }
        //if (mBackgroundImage = Engine->LoadTexture("textures/19F_reflection_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
        //if (mBackgroundImage = Engine->LoadTexture("textures/19F_Irradiance_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {

        mBackgroundImage = Engine->LoadTexture("textures/19F_Background_2K.texture.lea", LimitEngine::ArchiveFactory::ID, false);

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
    }

	virtual void FrameUpdate(double frameTime) override {
        //static LEMath::FloatVector3 gCameraPosition(0.0f, 1.5f, -3.0f);
        //mCamera->SetPosition(gCameraPosition);
        Engine->Update();
	}
private:
    LimitEngine::LimitEngine    *Engine;

    LimitEngine::ReferenceCountedPointer<LimitEngine::CinemaCamera>   mCamera;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Texture>        mBackgroundImage;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>          mSphereModel;
    uint32                                                            mSphereModelInstanceID;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>          mPlaneModel;
    uint32                                                            mPlaneModelInstanceID;
};

int main()
{    
    static constexpr size_t TotalMemory = 1024 << 20; // 1 GiB

	LimitEngine::MemoryAllocator::Init();
	LimitEngine::MemoryAllocator::InitWithMemoryPool(TotalMemory);
    LimitEngine::LimitEngine *Engine = new LimitEngine::LimitEngine();
    Engine->SetResourceRootPath("../../resources");

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