#include "dvs_pch.h"
#include "RenderCommand.h"

namespace Davos {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}
