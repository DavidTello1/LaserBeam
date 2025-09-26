#pragma once

namespace Davos {

	enum class ImageFormat 
	{
		NONE = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecs
	{
		uint32_t width = 1;
		uint32_t height = 1;
		ImageFormat format = ImageFormat::RGBA8;
		bool isGenerateMips = true;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual const TextureSpecs& GetSpecs() const = 0;

		virtual uint32_t GetRenderID() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual const std::string& GetPath() const = 0;
		virtual bool IsLoaded() const = 0;

		virtual bool operator ==(const Texture& other) const = 0;
	};

	// --- TEXTURE 2D ---
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecs& specs);
		static Ref<Texture2D> Create(const std::string& path);
	};

}
