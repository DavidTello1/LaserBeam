#include "dvs_pch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Davos {

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils {

		static GLenum TextureTarget(bool isMultisample) { 
			return (isMultisample) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; 
		}

		static void CreateTextures(bool isMultisample, uint32_t* outID, uint32_t count) {
			glCreateTextures(TextureTarget(isMultisample), count, outID);
		}

		static void BindTexture(bool isMultisample, uint32_t id) {
			glBindTexture(TextureTarget(isMultisample), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, GLint minFilter, GLint magFilter, GLint wrapR, GLint wrapS, GLint wrapT, uint32_t width, uint32_t height, int index) {
			bool isMultisample = (samples > 1);

			if (isMultisample)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(isMultisample), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, GLint minFilter, GLint magFilter, GLint wrapR, GLint wrapS, GLint wrapT, uint32_t width, uint32_t height) {
			bool isMultisample = (samples > 1);

			if (isMultisample)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(isMultisample), id, 0);
		}

		static bool IsDepthFormat(FB_TextureFormat format) {
			switch (format)
			{
				case FB_TextureFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

		static GLenum GetGLTextureFormat(FB_TextureFormat format) {
			switch (format)
			{
				case FB_TextureFormat::RGBA8:		return GL_RGBA8;
				case FB_TextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			DVS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum GetGLTextureFilterMin(FB_TextureFilterMin filterMin) {
			switch (filterMin)
			{
				case FB_TextureFilterMin::NEAREST:					return GL_NEAREST;
				case FB_TextureFilterMin::LINEAR:					return GL_LINEAR;
				case FB_TextureFilterMin::NEAREST_MIPMAP_NEAREST:	return GL_NEAREST_MIPMAP_NEAREST;
				case FB_TextureFilterMin::NEAREST_MIPMAP_LINEAR:	return GL_NEAREST_MIPMAP_LINEAR;
				case FB_TextureFilterMin::LINEAR_MIPMAP_NEAREST:	return GL_LINEAR_MIPMAP_NEAREST;
				case FB_TextureFilterMin::LINEAR_MIPMAP_LINEAR:		return GL_LINEAR_MIPMAP_LINEAR;
			}

			DVS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum GetGLTextureFilterMag(FB_TextureFilterMag filterMag) {
			switch (filterMag)
			{
			case FB_TextureFilterMag::NEAREST:	return GL_NEAREST;
			case FB_TextureFilterMag::LINEAR:	return GL_LINEAR;
			}

			DVS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum GetGLTextureWrap(FB_TextureWrap wrap) {
			switch (wrap)
			{
				case FB_TextureWrap::REPEAT:			return GL_REPEAT;
				case FB_TextureWrap::MIRRORED_REPEAT:	return GL_MIRRORED_REPEAT;
				case FB_TextureWrap::CLAMP_TO_EDGE:		return GL_CLAMP_TO_EDGE;
				case FB_TextureWrap::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
			}

			DVS_CORE_ASSERT(false);
			return 0;
		}

	}

	// --------------------------------------------------
	OpenGLFramebuffer::OpenGLFramebuffer(const FB_Specs& specs) : m_Specs(specs)
	{
		for (FB_TextureSpecs specs : m_Specs.attachments.attachments)
		{
			if (!Utils::IsDepthFormat(specs.textureFormat))
				m_ColorAttachmentSpecs.emplace_back(specs);
			else
				m_DepthAttachmentSpecs = specs;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RenderID);

		glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RenderID) // Reset FB if it exists
		{
			glDeleteFramebuffers(1, &m_RenderID);
			glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RenderID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RenderID);

		bool isMultisample = (m_Specs.samples > 1);

		// --- Color Attachments
		if (m_ColorAttachmentSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
			Utils::CreateTextures(isMultisample, m_ColorAttachments.data(), (uint32_t)m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
			{
				Utils::BindTexture(isMultisample, m_ColorAttachments[i]);
				
				FB_TextureSpecs textureSpecs = m_ColorAttachmentSpecs[i];
				switch (textureSpecs.textureFormat)
				{
					case FB_TextureFormat::RGBA8:
					{
						Utils::AttachColorTexture(m_ColorAttachments[i], 
							m_Specs.samples, 
							GL_RGBA8, 
							GL_RGBA, 
							Utils::GetGLTextureFilterMin(textureSpecs.filterMin),
							Utils::GetGLTextureFilterMag(textureSpecs.filterMag),
							Utils::GetGLTextureWrap(textureSpecs.wrapR),
							Utils::GetGLTextureWrap(textureSpecs.wrapS),
							Utils::GetGLTextureWrap(textureSpecs.wrapT),
							m_Specs.width, 
							m_Specs.height, 
							(int)i
						);
						break;
					}

					case FB_TextureFormat::RED_INTEGER:
					{
						Utils::AttachColorTexture(m_ColorAttachments[i], 
							m_Specs.samples, 
							GL_R32I, 
							GL_RED_INTEGER,
							Utils::GetGLTextureFilterMin(textureSpecs.filterMin),
							Utils::GetGLTextureFilterMag(textureSpecs.filterMag),
							Utils::GetGLTextureWrap(textureSpecs.wrapR),
							Utils::GetGLTextureWrap(textureSpecs.wrapS),
							Utils::GetGLTextureWrap(textureSpecs.wrapT),
							m_Specs.width, 
							m_Specs.height, 
							(int)i
						);
						break;
					}
				}
			}
		}

		// --- Depth Attachment
		if (m_DepthAttachmentSpecs.textureFormat != FB_TextureFormat::NONE)
		{
			Utils::CreateTextures(isMultisample, &m_DepthAttachment, 1);
			Utils::BindTexture(isMultisample, m_DepthAttachment);

			switch (m_DepthAttachmentSpecs.textureFormat)
			{
				case FB_TextureFormat::DEPTH24STENCIL8:
				{
					Utils::AttachDepthTexture(m_DepthAttachment, 
						m_Specs.samples, 
						GL_DEPTH24_STENCIL8, 
						GL_DEPTH_STENCIL_ATTACHMENT, 
						Utils::GetGLTextureFilterMin(m_DepthAttachmentSpecs.filterMin),
						Utils::GetGLTextureFilterMag(m_DepthAttachmentSpecs.filterMag),
						Utils::GetGLTextureWrap(m_DepthAttachmentSpecs.wrapR),
						Utils::GetGLTextureWrap(m_DepthAttachmentSpecs.wrapS),
						Utils::GetGLTextureWrap(m_DepthAttachmentSpecs.wrapT),
						m_Specs.width, 
						m_Specs.height
					);
					break;
				}
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			DVS_CORE_ASSERT(m_ColorAttachments.size() <= 4);

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only Depth Pass
			glDrawBuffer(GL_NONE);
		}

		DVS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Error Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RenderID);
		glViewport(0, 0, m_Specs.width, m_Specs.height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			DVS_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specs.width = width;
		m_Specs.height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		DVS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, int value)
	{
		DVS_CORE_ASSERT(index < m_ColorAttachments.size());

		FB_TextureSpecs& specs = m_ColorAttachmentSpecs[index];
		glClearTexImage(m_ColorAttachments[index], 0, Utils::GetGLTextureFormat(specs.textureFormat), GL_INT, &value);
	}

}
