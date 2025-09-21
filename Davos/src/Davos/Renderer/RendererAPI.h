#pragma once
#include "VertexArray.h"

#include <glm/glm.hpp>

namespace Davos {

	class RendererAPI
	{
	public:
		enum class API
		{
			NONE = 0,
			OPENGL = 1,
			//DIRECTX,
			//METAL,
			//VULKAN
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}
