#include "dvs_pch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Davos {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OPENGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateScope<OpenGLRendererAPI>();
		}

		DVS_CORE_ASSERT(false, "Error Creating RendererAPI: unknown RendererAPI");
		return nullptr;
	}

}
