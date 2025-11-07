#pragma once
#include "Davos/Renderer/Framebuffer.h"

namespace Davos {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FB_Specs& specs);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t index, int value) override;

		uint32_t GetColorAttachmentID(uint32_t index = 0) const override { 
			DVS_CORE_ASSERT(index < m_ColorAttachments.size()); 
			return m_ColorAttachments[index]; 
		}

		const FB_Specs& GetSpecification() const override { return m_Specs; }

	private:
		uint32_t m_RenderID = 0;
		FB_Specs m_Specs;

		std::vector<FB_TextureSpecs> m_ColorAttachmentSpecs;
		FB_TextureSpecs m_DepthAttachmentSpecs = { FB_TextureFormat::NONE, FB_TextureFilter::NONE, FB_TextureWrap::NONE };

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}
