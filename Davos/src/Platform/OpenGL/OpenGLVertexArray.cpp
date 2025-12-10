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

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RenderID);
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
			switch (element.type)
			{
				case ShaderDataType::FLOAT:
				case ShaderDataType::FLOAT_2:
				case ShaderDataType::FLOAT_3:
				case ShaderDataType::FLOAT_4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.type),
						element.isNormalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::INT:
				case ShaderDataType::INT_2:
				case ShaderDataType::INT_3:
				case ShaderDataType::INT_4:
				case ShaderDataType::BOOL:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribIPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.type),
						layout.GetStride(),
						(const void*)element.offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::MAT_3:
				case ShaderDataType::MAT_4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(m_VertexBufferIndex,
							count,
							ShaderDataTypeToOpenGLBaseType(element.type),
							element.isNormalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(element.offset + sizeof(float) * count * i));
						glVertexAttribDivisor(m_VertexBufferIndex, 1);
						m_VertexBufferIndex++;
					}
					break;
				}
				default:
					DVS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
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
