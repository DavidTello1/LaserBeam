#include "dvs_pch.h"
#include "Primitives.h"

#include "RenderCommand.h"
#include "Shader.h"

namespace Davos {

	// --------------------------------------
	// --- QUAD ---
	void Quad::Init(const uint32_t MaxVertices, const Ref<Shader>& shader)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(QuadVertex));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::FLOAT_3, "a_Position"     },
			{ ShaderDataType::FLOAT_4, "a_Color"        },
			{ ShaderDataType::FLOAT_2, "a_TexCoords"    },
			{ ShaderDataType::FLOAT,   "a_TexIndex"     },
			{ ShaderDataType::FLOAT,   "a_TilingFactor" },
			{ ShaderDataType::INT,     "a_EntityID"		}
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_VertexBufferBase = new QuadVertex[MaxVertices];

		static const uint32_t MaxIndices = MaxVertices * 6;
		uint32_t* quadIndices = new uint32_t[MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, MaxIndices);
		m_VertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		m_Shader = shader;
	}

	void Quad::CleanUp()
	{
		delete[] m_VertexBufferBase;
	}

	void Quad::Reset()
	{
		m_IndexCount = 0;
		m_VertexBufferPtr = m_VertexBufferBase;
	}

	bool Quad::Flush()
	{
		if (m_IndexCount == 0)
			return false;

		uint32_t dataSize = (uint32_t)((uint8_t*)m_VertexBufferPtr - (uint8_t*)m_VertexBufferBase);
		m_VertexBuffer->SetData(m_VertexBufferBase, dataSize);
		
		return true;
	}

	void Quad::SetVertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoords, const float texIndex, const float tilingFactor, const int entityID)
	{
		m_VertexBufferPtr->position = position;
		m_VertexBufferPtr->color = color;
		m_VertexBufferPtr->texCoords = texCoords;
		m_VertexBufferPtr->texIndex = texIndex;
		m_VertexBufferPtr->tilingFactor = tilingFactor;
		m_VertexBufferPtr->entityID = entityID;
		m_VertexBufferPtr++;
	}

	// --------------------------------------
	// --- LINE ---
	void Line::Init(const uint32_t MaxVertices, const Ref<Shader>& shader)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(LineVertex));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::FLOAT_3, "a_Position" },
			{ ShaderDataType::FLOAT_4, "a_Color"    },
			{ ShaderDataType::INT,     "a_EntityID"	}
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexBufferBase = new LineVertex[MaxVertices];

		m_Shader = shader;
	}

	void Line::CleanUp()
	{
		delete[] m_VertexBufferBase;
	}

	void Line::Reset()
	{
		m_VertexCount = 0;
		m_VertexBufferPtr = m_VertexBufferBase;
	}

	bool Line::Flush()
	{
		if (m_VertexCount == 0)
			return false;

		uint32_t dataSize = (uint32_t)((uint8_t*)m_VertexBufferPtr - (uint8_t*)m_VertexBufferBase);
		m_VertexBuffer->SetData(m_VertexBufferBase, dataSize);

		return true;
	}

	void Line::SetVertex(const glm::vec3& position, const glm::vec4& color, const int entityID)
	{
		m_VertexBufferPtr->position = position;
		m_VertexBufferPtr->color = color;
		m_VertexBufferPtr->entityID = entityID;
		m_VertexBufferPtr++;
	}

	// --------------------------------------
	void Primitive::BindShader()
	{
		m_Shader->Bind();
	}

}
