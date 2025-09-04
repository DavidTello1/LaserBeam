#include <Davos.h>

class ExampleLayer : public Davos::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
		//DVS_INFO("ExampleLayer::Update");
	}

	void OnEvent(Davos::Event& event) override {
		DVS_TRACE("{0}", event);
	}
};

// ---
class LaserBeam : public Davos::Application
{
public:
	LaserBeam() 
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Davos::ImGuiLayer());
	}

	~LaserBeam() 
	{
	}
};

// ----------------------------------
Davos::Application* Davos::CreateApplication()
{
	return new LaserBeam();
}
