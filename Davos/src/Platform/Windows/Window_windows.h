#pragma once
#include "Davos/Window.h"

struct GLFWwindow;

namespace Davos {

	class GraphicsContext;

	class WindowWindows : public Window
	{
	public:
		WindowWindows(const WindowProperties& properties);
		virtual ~WindowWindows();

		void OnUpdate() override;

		inline void* GetNativeWindow() const override { return m_Window; }

		inline unsigned int GetWidth() const override { return m_Data.width; }
		inline unsigned int GetHeight() const override { return m_Data.height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:
		virtual void Init(const WindowProperties& properties);
		virtual void CleanUp();

	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool isVSync;
			
			EventCallbackFn eventCallback;
		};

		WindowData m_Data;
	};

}
