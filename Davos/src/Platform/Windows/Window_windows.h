#pragma once
#include "Davos/Core/Window.h"

struct GLFWwindow;

namespace Davos {

	class GraphicsContext;

	class WindowWindows : public Window
	{
	public:
		WindowWindows(const WindowProps& properties);
		virtual ~WindowWindows();

		void OnUpdate() override;

		inline void* GetNativeWindow() const override { return m_Window; }

		inline uint32_t GetWidth() const override { return m_Data.width; }
		inline uint32_t GetHeight() const override { return m_Data.height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:
		void _Init(const WindowProps& properties);
		void _CleanUp();

	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string title;
			uint32_t width, height;
			bool isVSync;
			
			EventCallbackFn eventCallback;
		};

		WindowData m_Data;
	};

}
