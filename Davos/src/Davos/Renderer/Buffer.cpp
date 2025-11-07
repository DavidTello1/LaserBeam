#include "dvs_pch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Davos {

	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLVertexBuffer>(size);
		}

		DVS_CORE_ASSERT(false, "Error creating VertexBuffer: unknown RendererAPI");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, size_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		DVS_CORE_ASSERT(false, "Error creating VertexBuffer: unknown RendererAPI");
		return nullptr;
	}


	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		DVS_CORE_ASSERT(false, "Error creating IndexBuffer: unknown RendererAPI");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		DVS_CORE_ASSERT(false, "Error creating UniformBuffer: unknown RendererAPI");
		return nullptr;
	}

}
