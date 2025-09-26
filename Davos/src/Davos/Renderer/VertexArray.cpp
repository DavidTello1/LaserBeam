#include "dvs_pch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Davos {

    Ref<VertexArray> VertexArray::Create()
    {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLVertexArray>();
		}

		DVS_CORE_ASSERT(false, "Error creating VertexArray: unknown RendererAPI");
		return nullptr;
	}

}
