#include "dvs_pch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Davos {

    VertexArray* VertexArray::Create()
    {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:	return new OpenGLVertexArray();
		}

		DVS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
