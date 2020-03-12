#include <TBApplication.h>
#include <TBWidget.h>

#include <LimitEngine.h>

class LimitEngineWindow : public ToolBase::TBWidget
{
public:
	LimitEngineWindow(const char *Title, LimitEngine::LimitEngine *InEngine) 
        : ToolBase::TBWidget(Title, 640, 480) 
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

	ToolBase::TBApplication app;
	LimitEngineWindow window("Test", Engine);
    Engine->Init(window.GetHandle());
	app.Run();
    Engine->Term();
	window.~LimitEngineWindow();

	delete Engine;
	LimitEngine::MemoryAllocator::Term();
}