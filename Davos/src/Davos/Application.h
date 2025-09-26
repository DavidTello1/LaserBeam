#pragma once
#include "Core/Window.h"
#include "Core/LayerStack.h"

namespace Davos {

	class ImGuiLayer;
	class Event;
	class WindowCloseEvent;

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void OnEvent(Event& e);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		bool m_isRunning = true;
		float m_LastFrameTime = 0.0f;

		Scope<Window> m_Window;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
