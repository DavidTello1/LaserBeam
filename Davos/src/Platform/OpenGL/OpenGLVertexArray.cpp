#include "dvs_pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Davos {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::FLOAT:		return GL_FLOAT;
			case ShaderDataType::FLOAT_2:	return GL_FLOAT;
			case ShaderDataType::FLOAT_3:	return GL_FLOAT;
			case ShaderDataType::FLOAT_4:	return GL_FLOAT;
			case ShaderDataType::INT:		return GL_INT;
			case ShaderDataType::INT_2:		return GL_INT;
			case ShaderDataType::INT_3:		return GL_INT;
			case ShaderDataType::INT_4:		return GL_INT;
			case ShaderDataType::MAT_3:		return GL_FLOAT;
			case ShaderDataType::MAT_4:		return GL_FLOAT;
			case ShaderDataType::BOOL:		return GL_BOOL;
		}

		DVS_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RenderID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RenderID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		DVS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RenderID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const BufferLayout& layout = vertexBuffer->GetLayout();
		for (const BufferElement& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.isNormalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset
			);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RenderID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}
