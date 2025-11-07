#include "dvs_pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Davos {

	struct RendererData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Quad Quad;
		Line Line;
		//Circle Circle;
		//Text Text;

		float lineWidth = 1.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 = white texture

		Ref<Texture2D> whiteTexture;
		//Ref<Texture2D> fontAtlasTexture;
		
		glm::vec4 quadVertexPositions[4];

		struct CameraData {
			glm::mat4 viewProjection;
		};
		CameraData cameraBuffer;
		Ref<UniformBuffer> cameraUniformBuffer;

		Renderer::Statistics stats;
	};

	static RendererData s_Data;

	// --------------------------------------------------
	void Renderer::Init()
	{
		RenderCommand::Init();

		s_Data.Quad.Init(s_Data.MaxVertices, Shader::Create("assets/shaders/Quad.glsl"));
		s_Data.Line.Init(s_Data.MaxVertices, Shader::Create("assets/shaders/Line.glsl"));
		//s_Data.Circle.Init(s_Data.MaxVertices, Shader::Create("assets/shaders/Circle.glsl"));
		//s_Data.Text.Init(s_Data.MaxVertices, Shader::Create("assets/shaders/Text.glsl"));

		s_Data.whiteTexture = Texture2D::Create(TextureSpecs());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		// Set first texture slot to WhiteTexture
		s_Data.textureSlots[0] = s_Data.whiteTexture;

		s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.cameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
	}

	void Renderer::CleanUp()
	{
		s_Data.Quad.CleanUp();
		s_Data.Line.CleanUp();
		//s_Data.Circle.CleanUp();
		//s_Data.Text.CleanUp();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.cameraBuffer.viewProjection = camera.GetProjectionMatrix() * glm::inverse(transform);
		s_Data.cameraUniformBuffer->SetData(&s_Data.cameraBuffer, sizeof(RendererData::CameraData));

		StartBatch();
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_Data.cameraBuffer.viewProjection = camera.GetViewProjectionMatrix();
		s_Data.cameraUniformBuffer->SetData(&s_Data.cameraBuffer, sizeof(RendererData::CameraData));

		StartBatch();
	}


	void Renderer::BeginScene(/*const EditorCamera& camera*/)
	{
		//s_Data.cameraBuffer.viewProjection = camera.GetViewProjectionMatrix();
		//s_Data.cameraUniformBuffer->SetData(&s_Data.cameraBuffer, sizeof(RendererData::CameraData));

		StartBatch();
	}

	void Renderer::EndScene()
	{
		RenderBatch();
	}

	void Renderer::StartBatch()
	{
		s_Data.Quad.Reset();
		s_Data.Line.Reset();
		//s_Data.Circle.Reset();
		//s_Data.Text.Reset();
	}

	void Renderer::RenderBatch()
	{
		if (s_Data.Quad.Flush())
		{
			// Bind textures
			for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
				s_Data.textureSlots[i]->Bind(i);

			s_Data.Quad.BindShader();
			RenderCommand::DrawIndexed(s_Data.Quad.GetVertexArray(), s_Data.Quad.GetIndexCount());

			s_Data.stats.drawCalls++;
		}

		if (s_Data.Line.Flush())
		{
			s_Data.Line.BindShader();
			RenderCommand::SetLineWidth(s_Data.lineWidth);
			RenderCommand::DrawLines(s_Data.Line.GetVertexArray(), s_Data.Line.GetVertexCount());

			s_Data.stats.drawCalls++;
		}

		//if (s_Data.Circle.Draw())
		//	s_Data.Stats.DrawCalls++;

		//if (s_Data.Text.Draw())
		//	s_Data.Stats.DrawCalls++;
	}

	void Renderer::NextBatch()
	{
		RenderBatch();
		StartBatch();
	}

	// --------------------------------------------------
	// --- DRAW ---
	void Renderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
	{
		s_Data.Line.SetVertex(start, color);
		s_Data.Line.SetVertex(end, color);

		s_Data.Line.Add();

		s_Data.stats.lineCount++;
	}

	void Renderer::DrawRect(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& borderColor)
	{
		if (borderColor.a == 0.0f)
			return;

		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		if (rotation != 0.0f)
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });

			p0 = glm::vec3(rot * glm::vec4(p0, 1.0f));
			p1 = glm::vec3(rot * glm::vec4(p1, 1.0f));
			p2 = glm::vec3(rot * glm::vec4(p2, 1.0f));
			p3 = glm::vec3(rot * glm::vec4(p3, 1.0f));
		}

		DrawLine(p0, p1, borderColor);
		DrawLine(p1, p2, borderColor);
		DrawLine(p2, p3, borderColor);
		DrawLine(p3, p0, borderColor);
	}

	void Renderer::DrawRectFilled(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& backgroundColor, const glm::vec4& borderColor)
	{
		DrawQuad(position, rotation, size, backgroundColor);

		if (borderColor.a > 0.0f)
			DrawRect(position, rotation, size, borderColor);
	}

	void Renderer::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0.0f)
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		constexpr size_t QuadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		if (s_Data.Quad.GetIndexCount() >= RendererData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.Quad.SetVertex(
				transform * s_Data.quadVertexPositions[i],
				color,
				textureCoords[i],
				textureIndex,
				tilingFactor
			);
		}

		s_Data.Quad.Add();

		s_Data.stats.quadCount++;
	}

	void Renderer::DrawSprite(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0.0f)
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawSprite(transform, texture, tintColor, tilingFactor);
	}

	void Renderer::DrawSprite(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor)
	{
		constexpr size_t QuadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data.Quad.GetIndexCount() >= RendererData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.textureSlotIndex >= RendererData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}

		for (size_t i = 0; i < QuadVertexCount; i++)
		{
			s_Data.Quad.SetVertex(
				transform * s_Data.quadVertexPositions[i],
				tintColor,
				textureCoords[i],
				textureIndex,
				tilingFactor
			);
		}

		s_Data.Quad.Add();

		s_Data.stats.quadCount++;
	}

	// --------------------------------------------------
	// --- LINE WIDTH ---

	float Renderer::GetLineWidth()
	{
		return s_Data.lineWidth;
	}

	void Renderer::SetLineWidth(float width)
	{
		s_Data.lineWidth = width;
	}

	// --------------------------------------------------
	// --- DEBUG STATS ---
	void Renderer::ResetStats()
	{
		memset(&s_Data.stats, 0, sizeof(Statistics));
	}

	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.stats;
	}

}
