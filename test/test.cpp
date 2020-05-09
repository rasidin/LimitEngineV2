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
	
    void SetupScene() {
        mCamera = new LimitEngine::CinemaCamera();
        mCamera->SetPosition( LEMath::FloatVector3(0.0f, 0.0f,-5.0f));
        mCamera->SetDirection(LEMath::FloatVector3(0.0f, 0.0f, 1.0f));
        mCamera->SetShutterSpeed(1.0f / 8.0f);
        mCamera->SetFStop(1.0f / 4.0f);
        mCamera->SetExposureOffset(-3.0f);
        //mCamera->SetExposureOffset(0.0f);
        Engine->SetMainCamera(mCamera.Get());

        if (LimitEngine::TextureFactory *Factory = (LimitEngine::TextureFactory*)LimitEngine::ResourceManager::GetSingleton().GetFactory(LimitEngine::TextureFactory::ID)) {
            Factory->SetImportFilter(LimitEngine::TextureFactory::TextureImportFilter::Irradiance);
        }

        {
            LimitEngine::LightIBL *DefaultIBL = new LimitEngine::LightIBL();
            if (LimitEngine::Texture *IBLReflectionTexture = Engine->LoadTexture("textures/19F_Reflection_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLReflectionTexture->InitResource();
                DefaultIBL->SetIBLReflectionTexture(IBLReflectionTexture);
            }
            if (LimitEngine::Texture *IBLIrradianceTexture = Engine->LoadTexture("textures/19F_Irradiance_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
                IBLIrradianceTexture->InitResource();
                DefaultIBL->SetIBLIrradianceTexture(IBLIrradianceTexture);
            }
            Engine->AddLight(DefaultIBL);
        }

        //mSphereModel = Engine->LoadModel("models/sphere.model.text", LimitEngine::ModelFactory::ID, false);
        mSphereModel = Engine->LoadModel("models/sphere.model.lea", LimitEngine::ArchiveFactory::ID, false);
        if (mSphereModel.IsValid()) 
        {
            mSphereModel->InitResource();
        }
        //if (mBackgroundImage = Engine->LoadTexture("textures/19F_reflection_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
        //if (mBackgroundImage = Engine->LoadTexture("textures/19F_Irradiance_4K.texture.lea", LimitEngine::ArchiveFactory::ID, false)) {
        mBackgroundImage = Engine->LoadTexture("textures/19F_Background_2K.texture.lea", LimitEngine::ArchiveFactory::ID, false);
        //if (mBackgroundImage = Engine->LoadTexture("textures/Alexs_Apt_2k.tga", LimitEngine::TextureFactory::ID, false)) {
        if (mBackgroundImage.IsValid()) {
            mBackgroundImage->InitResource();
            Engine->SetBackgroundImage(mBackgroundImage, LimitEngine::BackgroundImageType::Fullscreen);
        }
    }

	virtual void FrameUpdate(double frameTime) override {
        Engine->Update();
	}
private:
    LimitEngine::LimitEngine    *Engine;

    LimitEngine::ReferenceCountedPointer<LimitEngine::CinemaCamera>   mCamera;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Texture>        mBackgroundImage;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>          mSphereModel;
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