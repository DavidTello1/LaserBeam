#pragma once
#include <glm/glm.hpp>

namespace Davos {

	class VertexArray;
	class VertexBuffer;
	class Shader;

	class Primitive
	{
	public:
		~Primitive() = default;

		virtual void Init(const uint32_t MaxVertices, const Ref<Shader>& shader) = 0;
		virtual void CleanUp() = 0;
		virtual void Reset() = 0;
		virtual bool Flush() = 0;

		virtual void Add() = 0;

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		void BindShader();

	protected:
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<Shader> m_Shader;
	};

	// --- QUAD ---
	class Quad : public Primitive
	{
		struct QuadVertex {
			glm::vec3 position;
			glm::vec4 color;
			glm::vec2 texCoords;

			float texIndex;
			float tilingFactor;
		};

	public:
		void Init(const uint32_t MaxVertices, const Ref<Shader>& shader) override;
		void CleanUp() override;
		void Reset() override;
		bool Flush() override;

		void Add() override { m_IndexCount += 6; }

		const uint32_t GetIndexCount() const { return m_IndexCount; }
		void SetVertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoords, const float texIndex, const float tilingFactor);
	
	private:
		uint32_t m_IndexCount = 0;
		QuadVertex* m_VertexBufferBase = nullptr;
		QuadVertex* m_VertexBufferPtr = nullptr;
	};

	// --- LINE ---
	class Line : public Primitive
	{
		struct LineVertex {
			glm::vec3 position;
			glm::vec4 color;
		};

	public:
		void Init(const uint32_t MaxVertices, const Ref<Shader>& shader) override;
		void CleanUp() override;
		void Reset() override;
		bool Flush() override;

		void Add() override { m_VertexCount += 2; }

		const uint32_t GetVertexCount() const { return m_VertexCount; }
		void SetVertex(const glm::vec3& position, const glm::vec4& color);

	private:
		uint32_t m_VertexCount = 0;
		LineVertex* m_VertexBufferBase = nullptr;
		LineVertex* m_VertexBufferPtr = nullptr;
	};

	//// --- CIRCLE ---
	//struct CircleData
	//{
	//	glm::vec2 WorldPosition;
	//	glm::vec2 LocalPosition;
	//	glm::vec4 Color;
	//	float Thickness;
	//	float Fade;

	//	float Layer;
	//};

	//// --- TEXT ---
	//struct TextData
	//{
	//	glm::vec2 Position;
	//	glm::vec4 Color;
	//	glm::vec2 TexCoords;

	//	glm::vec4 BackgroundColor;
	//	glm::vec4 BorderColor;

	//	float Layer;
	//};
}
