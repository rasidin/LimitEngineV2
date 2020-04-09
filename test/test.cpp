#include <TBApplication.h>
#include <TBWidget.h>

#include <LimitEngine.h>
#include <Factories/TextureFactory.h>
#include <Managers/ResourceManager.h>
#include <Renderer/Texture.h>

#include <iostream>

class LimitEngineWindow : public ToolBase::TBWidget
{
public:
	LimitEngineWindow(const char *Title, LimitEngine::LimitEngine *InEngine) 
        : ToolBase::TBWidget(Title, 1920, 1080) 
        , Engine(InEngine)
        , mBackgroundImage(nullptr)
    {}
	
    void SetupScene() {
        if (LimitEngine::ResourceManager::RESOURCE *TextureResource = LimitEngine::LE_ResourceManager.GetResourceWithoutRegister("textures/19F_Background_2K.exr", LimitEngine::TextureFactory::ID)) {
            mBackgroundImage = static_cast<LimitEngine::Texture*>(TextureResource->data);
            mBackgroundImage->InitResource();
            Engine->SetBackgroundImage(mBackgroundImage, LimitEngine::LimitEngine::BackgroundImageType::FullScreen);
        }
    }

	virtual void FrameUpdate(double frameTime) override {
        Engine->Update();
	}
private:
    LimitEngine::LimitEngine *Engine;

    LimitEngine::Texture *mBackgroundImage;
};

int main()
{    
    static constexpr size_t TotalMemory = 256 << 20; // 256MiB

	LimitEngine::MemoryAllocator::Init();
	LimitEngine::MemoryAllocator::InitWithMemoryPool(TotalMemory);
    LimitEngine::LimitEngine *Engine = new LimitEngine::LimitEngine();
    Engine->SetResourceRootPath("../../resources");

	ToolBase::TBApplication app;
	LimitEngineWindow window("Test", Engine);
    Engine->Init(window.GetHandle());
    window.SetupScene();
	app.Run();
    Engine->Term();
	window.~LimitEngineWindow();

	delete Engine;
	LimitEngine::MemoryAllocator::Term();

    return 0;
}