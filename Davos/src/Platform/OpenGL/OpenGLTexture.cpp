#include "dvs_pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION  // Must be defined only once
#include <stb/stb_image.h>

namespace Davos {

	namespace Utils {

		static GLenum DavosImgFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:	 return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}
			DVS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum DavosImgFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:	 return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
			}
			DVS_CORE_ASSERT(false);
			return 0;
		}

	}

	// --------------------------------------------------
	// --- TEXTURE 2D ---
	OpenGLTexture2D::OpenGLTexture2D(const std::string& name, const TextureSpecs& specs) : m_Name(name), m_Specs(specs), m_Width(specs.width), m_Height(specs.height)
	{
		m_InternalFormat = Utils::DavosImgFormatToGLInternalFormat(m_Specs.format);
		m_DataFormat = Utils::DavosImgFormatToGLDataFormat(m_Specs.format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RenderID);
		glTextureStorage2D(m_RenderID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RenderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RenderID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& name, const std::string& path) : m_Name(name), m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = nullptr;
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			DVS_CORE_ERROR("Unable to load Texture2D from path: {0}", path);
			return;
		}

		m_IsLoaded = true;
		m_Width = width;
		m_Height = height;

		m_InternalFormat = 0;
		m_DataFormat = 0;
		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}
		//*** channels == 1

		DVS_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Error loading Texture2D: Format not supported");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RenderID);
		glTextureStorage2D(m_RenderID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RenderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //*** filter in specs
		glTextureParameteri(m_RenderID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_S, GL_REPEAT); //*** wrap mode in specs
		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RenderID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RenderID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t channels = (m_DataFormat == GL_RGBA) ? 4 : 3; //*** channels == 1
		DVS_CORE_ASSERT(size == m_Width * m_Height * channels, "Data must be entire texture!");

		glTextureSubImage2D(m_RenderID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RenderID);
	}

}
