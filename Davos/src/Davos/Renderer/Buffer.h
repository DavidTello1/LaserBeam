#pragma once

namespace Davos {

	enum class ShaderDataType
	{
		NONE = 0, 
		FLOAT, FLOAT_2, FLOAT_3, FLOAT_4, 
		INT, INT_2, INT_3, INT_4, 
		MAT_3, MAT_4, 
		BOOL
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::FLOAT:		return 4;
			case ShaderDataType::FLOAT_2:	return 4 * 2;
			case ShaderDataType::FLOAT_3:	return 4 * 3;
			case ShaderDataType::FLOAT_4:	return 4 * 4;
			case ShaderDataType::INT:		return 4;
			case ShaderDataType::INT_2:		return 4 * 2;
			case ShaderDataType::INT_3:		return 4 * 3;
			case ShaderDataType::INT_4:		return 4 * 4;
			case ShaderDataType::MAT_3:		return 4 * 3 * 3;
			case ShaderDataType::MAT_4:		return 4 * 4 * 4;
			case ShaderDataType::BOOL:		return 1;	
		}

		DVS_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		size_t offset;
		bool isNormalized;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool isNormalized = false)
			: type(type), name(name), size(ShaderDataTypeSize(type)), offset(0), isNormalized(isNormalized) 
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
				case ShaderDataType::FLOAT:		return 1;
				case ShaderDataType::FLOAT_2:	return 2;
				case ShaderDataType::FLOAT_3:	return 3;
				case ShaderDataType::FLOAT_4:	return 4;
				case ShaderDataType::INT:		return 1;
				case ShaderDataType::INT_2:		return 2;
				case ShaderDataType::INT_3:		return 3;
				case ShaderDataType::INT_4:		return 4;
				case ShaderDataType::MAT_3:		return 3; // 3 * float3
				case ShaderDataType::MAT_4:		return 4; // 4 * float4
				case ShaderDataType::BOOL:		return 1;
			}

			DVS_CORE_ASSERT(false, "Unknown ShaderDataType");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (BufferElement& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}
	private:
		uint32_t m_Stride = 0;
		std::vector<BufferElement> m_Elements;
	};

	// --- VERTEX BUFFER ---
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, size_t size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(size_t size);
		static Ref<VertexBuffer> Create(float* vertices, size_t size);
	};

	// --- INDEX BUFFER ---
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}
