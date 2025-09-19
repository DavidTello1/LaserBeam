#pragma once
#include "RendererAPI.h"

namespace Davos {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) {
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};

}
