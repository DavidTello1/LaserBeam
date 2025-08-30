#pragma once
#include "Core.h"
#include "Window.h"

namespace Davos {

	class Event;
	class WindowCloseEvent;

	class DAVOS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_isRunning = true;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}
