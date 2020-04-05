#include <TBApplication.h>
#include <TBWidget.h>

#include <LimitEngine.h>

#include <iostream>

class LimitEngineWindow : public ToolBase::TBWidget
{
public:
	LimitEngineWindow(const char *Title, LimitEngine::LimitEngine *InEngine) 
        : ToolBase::TBWidget(Title, 1920, 1080) 
        , Engine(InEngine)
    {}
	
	virtual void FrameUpdate(double frameTime) override {
        Engine->Update();
	}
private:
    LimitEngine::LimitEngine *Engine;
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
	app.Run();
    Engine->Term();
	window.~LimitEngineWindow();

	delete Engine;
	LimitEngine::MemoryAllocator::Term();

    return 0;
}