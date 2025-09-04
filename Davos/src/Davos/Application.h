#pragma once
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace Davos {

	class Event;
	class WindowCloseEvent;

	class DAVOS_API Application
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
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
