#pragma once

namespace Davos {

	enum class FB_TextureFormat 
	{
		NONE = 0,
		RGBA8,					// Color (4 channels)
		RED_INTEGER,			// Color (1 channel)
		DEPTH24STENCIL8,		// Depth / Stencil
		DEPTH = DEPTH24STENCIL8 // Defaults
	};

	enum class FB_TextureFilter //***
	{
		NONE = 0,
		LINEAR,
		REPEAT
	};

	enum class FB_TextureWrap //***
	{
		NONE = 0,
		LINEAR,
		REPEAT
	};

	struct FB_TextureSpecs
	{
		FB_TextureSpecs() = default;
		FB_TextureSpecs(FB_TextureFormat format, FB_TextureFilter filter, FB_TextureWrap wrap)
			: textureFormat(format), filterMin(filter), filterMag(filter), wrapR(wrap), wrapS(wrap), wrapT(wrap) {}

		FB_TextureSpecs(FB_TextureFormat format, FB_TextureFilter filterMin, FB_TextureFilter filterMag, FB_TextureWrap wrapR, FB_TextureWrap wrapS, FB_TextureWrap wrapT)
			: textureFormat(format), filterMin(filterMin), filterMag(filterMag), wrapR(wrapR), wrapS(wrapS), wrapT(wrapT) {}

		// ---
		FB_TextureFormat textureFormat = FB_TextureFormat::NONE;

		FB_TextureFilter filterMin = FB_TextureFilter::NONE;
		FB_TextureFilter filterMag = FB_TextureFilter::NONE;

		FB_TextureWrap wrapR = FB_TextureWrap::NONE;
		FB_TextureWrap wrapS = FB_TextureWrap::NONE;
		FB_TextureWrap wrapT = FB_TextureWrap::NONE;
	};


	struct FB_AttachmentSpecs
	{
		FB_AttachmentSpecs() = default;
		FB_AttachmentSpecs(std::initializer_list<FB_TextureSpecs> attachments) : attachments(attachments) {}

		std::vector<FB_TextureSpecs> attachments;
	};

	struct FB_Specs
	{
		uint32_t width = 0;
		uint32_t height = 0;

		FB_AttachmentSpecs attachments;
		uint32_t samples = 1;

		bool isSwapChainTarget = false;
	};

	// -----------------------------------------------
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentID, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t index, int value) = 0;

		virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const = 0;

		virtual const FB_Specs& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FB_Specs& specs);
	};

}
