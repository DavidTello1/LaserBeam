#include "dvs_pch.h"
#include "Framebuffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Davos {

	Ref<Framebuffer> Framebuffer::Create(const FB_Specs& specs)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLFramebuffer>(specs);
		}

		DVS_CORE_ASSERT(false, "Error creating Framebuffer: unknown RendererAPI");
		return nullptr;
	}

}
