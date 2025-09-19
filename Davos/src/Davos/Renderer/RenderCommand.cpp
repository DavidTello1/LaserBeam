#include "dvs_pch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Davos {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}
