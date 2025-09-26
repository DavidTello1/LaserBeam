#include "dvs_pch.h"
#include "GraphicsContext.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Davos {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:    DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		DVS_CORE_ASSERT(false, "Error creating GraphicsContext: unknown RendererAPI");
		return nullptr;
	}

}
