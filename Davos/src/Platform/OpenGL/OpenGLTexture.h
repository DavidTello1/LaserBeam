#pragma once
#include "Davos/Renderer/Texture.h"

namespace Davos {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecs& specs);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
		
		void SetData(void* data, uint32_t size) override;

		void Bind(uint32_t slot = 0) const override;

		const TextureSpecs& GetSpecs() const override { return m_Specs; }

		uint32_t GetRenderID() const override { return m_RenderID; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		const std::string& GetPath() const override { return m_Path; }
		bool IsLoaded() const override { return m_IsLoaded; }

		bool operator ==(const Texture& other) const override {
			return (m_RenderID == other.GetRenderID());
		}

	private:
		TextureSpecs m_Specs;

		uint32_t m_RenderID;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		std::string m_Path;
		bool m_IsLoaded = false;

		unsigned int m_InternalFormat = 0;
		unsigned int m_DataFormat = 0;
	};

}
