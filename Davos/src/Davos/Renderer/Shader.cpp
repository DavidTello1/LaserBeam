#include "dvs_pch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Davos {

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLShader>(filepath);
		}

		DVS_CORE_ASSERT(false, "Error creating Shader: unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::NONE:	DVS_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported"); return nullptr;
			case RendererAPI::API::OPENGL:	return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
		}

		DVS_CORE_ASSERT(false, "Error creating Shader: unknown RendererAPI");
		return nullptr;
	}

	// --------------------------------------------------
	// --- SHADER LIBRARY ---
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		DVS_CORE_ASSERT(!Exists(name), "Shader already exists");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		const std::string& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		Add(shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		Add(name, shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		DVS_CORE_ASSERT(Exists(name), "Shader not found with name: {0}", name);
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return (m_Shaders.find(name) != m_Shaders.end());
	}

}
