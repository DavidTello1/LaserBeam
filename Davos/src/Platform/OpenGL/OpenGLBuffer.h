#pragma once
#include "Davos/Renderer/Buffer.h"

namespace Davos {

	// --- VERTEX BUFFER ---
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(size_t size);
		OpenGLVertexBuffer(float* vertices, size_t size);
		virtual ~OpenGLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, size_t size) override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RenderID;
		BufferLayout m_Layout;
	};

	// --- INDEX BUFFER ---
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_RenderID;
		uint32_t m_Count;
	};

	// --- UNIFORM BUFFER ---
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t bindings);
		virtual ~OpenGLUniformBuffer();

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t m_RenderID;
	};

}
