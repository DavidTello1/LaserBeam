#pragma once
#include "Davos/Renderer/Shader.h"

#include <glm/glm.hpp>

namespace Davos {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		
		virtual ~OpenGLShader();

		void Bind() const override;
		void UnBind() const override;

		const std::string& GetName() const override { return m_Name; }

		void SetInt(const std::string& name, int value) override						{ _UploadUniformInt(name, value); }
		void SetIntArray(const std::string& name, int* values, uint32_t count) override { _UploadUniformIntArray(name, values, count); }
		void SetFloat(const std::string& name, float value) override					{ _UploadUniformFloat(name, value); }
		void SetFloat2(const std::string& name, const glm::vec2& value) override		{ _UploadUniformFloat2(name, value); }
		void SetFloat3(const std::string& name, const glm::vec3& value) override		{ _UploadUniformFloat3(name, value); }
		void SetFloat4(const std::string& name, const glm::vec4& value) override		{ _UploadUniformFloat4(name, value); }
		void SetMat3(const std::string& name, const glm::mat3& value) override			{ _UploadUniformMat3(name, value); }
		void SetMat4(const std::string& name, const glm::mat4& value) override			{ _UploadUniformMat4(name, value); }

	private:
		std::string _ReadFile(const std::string& filepath);
		std::unordered_map<unsigned int, std::string> _GetSourcesFromFile(const std::string& file);
		void _Compile(const std::unordered_map<unsigned int, std::string>& shaderSources);

		void _UploadUniformInt(const std::string& name, int value);
		void _UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void _UploadUniformFloat(const std::string& name, float value);
		void _UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void _UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void _UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void _UploadUniformMat3(const std::string& name, const glm::mat3& value);
		void _UploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		uint32_t m_RenderID;

		std::string m_FilePath;
		std::string m_Name;
	};

}
