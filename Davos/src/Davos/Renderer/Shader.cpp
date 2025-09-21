#include "dvs_pch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Davos {

	Shader* Shader::Create(const std::string& vertexSource, const std::string& fragmentSource)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OPENGL:	return new OpenGLShader(vertexSource, fragmentSource);
		}

		DVS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
