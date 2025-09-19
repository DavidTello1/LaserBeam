#pragma once
#include "Davos/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Davos {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};

}
