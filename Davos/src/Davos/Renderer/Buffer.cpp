#include "dvs_pch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Davos {

	VertexBuffer* VertexBuffer::Create(size_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:	return new OpenGLVertexBuffer(size);
		}

		DVS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	VertexBuffer* VertexBuffer::Create(float* vertices, size_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OPENGL:	return new OpenGLVertexBuffer(vertices, size);
		}

		DVS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OPENGL:	return new OpenGLIndexBuffer(indices, count);
		}

		DVS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
