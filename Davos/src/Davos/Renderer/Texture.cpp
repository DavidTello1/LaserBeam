#include "dvs_pch.h"
#include "Texture.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Davos {

	Ref<Texture2D> Texture2D::Create(const TextureSpecs& specs)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLTexture2D>(specs);
		}

		DVS_CORE_ASSERT(false, "Error creating Texture2D: unknown RendererAPI");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLTexture2D>(path);
		}

		DVS_CORE_ASSERT(false, "Error creating Texture2D: unknown RendererAPI");
		return nullptr;
	}

}
