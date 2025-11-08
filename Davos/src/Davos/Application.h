#pragma once
#include "Core/Window.h"
#include "Core/LayerStack.h"

int main(int argc, char** argv);

namespace Davos {

	class ImGuiLayer;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;

	struct AppCommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		const char* operator[](int index) const
		{
			DVS_CORE_ASSERT(index < count);
			return args[index];
		}
	};

	struct AppSpecs
	{
		std::string name = "Davos App";
		std::string workingDir;
		AppCommandLineArgs commandLineArgs;
	};

	class Application
	{
	public:
		Application(const AppSpecs& specs);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		Window& GetWindow() { return *m_Window; }
		const AppSpecs& GetSpecs() const { return m_Specs; }

		inline static Application& Get() { return *s_Instance; }

	private:
		void Run();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		AppSpecs m_Specs;

		Scope<Window> m_Window;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		bool m_isRunning = true;
		bool m_isMinimized = false;

		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in CLIENT
	Application* CreateApplication(AppCommandLineArgs args);

}
