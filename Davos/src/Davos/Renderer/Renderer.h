#pragma once
#include "RenderCommand.h"
#include "Primitives.h"

namespace Davos {
	
	class Camera;
	//class EditorCamera;
	class Texture2D;

	class Renderer
	{
	public:
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void Init();
		static void CleanUp();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		//static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void RenderBatch();

		static float GetLineWidth();
		static void SetLineWidth(float width);

		// --- Draw
		static void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);

		static void DrawSprite(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawSprite(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
		
		//static void DrawTile(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& tileSize, const glm::vec2& cellSize, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);		
		static void DrawRect(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& borderColor);

		static void DrawRectFilled(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& backgroundColor, const glm::vec4& borderColor = glm::vec4(0.0f));
		
		//static void DrawText(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& params);

		// --- Debug
		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;
			uint32_t lineCount = 0;

			uint32_t GetTotalVertexCount() const { return (quadCount * 4) + (lineCount * 2); }
			uint32_t GetTotalIndexCount() const { return quadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};

}
