#include "dvs_pch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Davos {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		DVS_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DVS_CORE_ASSERT(status, "Failed to initialize Glad!");

		DVS_CORE_INFO("OpenGL Info:");
		DVS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		DVS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		DVS_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		DVS_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Davos requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}