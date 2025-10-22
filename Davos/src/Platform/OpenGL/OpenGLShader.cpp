#include "dvs_pch.h"
#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Davos {

	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			DVS_CORE_ASSERT(false, "Unknown shader type");
			return 0;
		}

		static std::string GetNameFromFilePath(const std::string& filepath)
		{
			size_t lastSlash = filepath.find_last_of("/\\");
			lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

			size_t lastDot = filepath.rfind('.');
			size_t count = (lastDot == std::string::npos) ? filepath.size() - lastSlash : lastDot - lastSlash;

			return (filepath.substr(lastSlash, count));
		}
	
	}

	// --------------------------------------------------
	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string file = _ReadFile(filepath);
		std::unordered_map<unsigned int, std::string> sources = _GetSourcesFromFile(file);
		_Compile(sources);

		m_Name = Utils::GetNameFromFilePath(filepath);
		m_FilePath = filepath;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		_Compile(sources);

		m_Name = name;
		m_FilePath = "";
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RenderID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RenderID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	// --------------------------------------------------
	std::string OpenGLShader::_ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII

		if (!in)
		{
			DVS_CORE_ERROR("Could not open file '{0}'", filepath);
			return result;
		}

		in.seekg(0, std::ios::end);

		size_t size = in.tellg();
		if (size == -1)
		{
			DVS_CORE_ERROR("Could not read from file '{0}'", filepath);
			return result;
		}
		
		result.resize(size);
		in.seekg(0, std::ios::beg);
		in.read(&result[0], size);

		return result;
	}

	std::unordered_map<unsigned int, std::string> OpenGLShader::_GetSourcesFromFile(const std::string& file)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = file.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = file.find_first_of("\r\n", pos); //End of shader type declaration line
			DVS_CORE_ASSERT(eol != std::string::npos, "Syntax error");

			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = file.substr(begin, eol - begin);
			DVS_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = file.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			DVS_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");

			pos = file.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? file.substr(nextLinePos) : file.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::_Compile(const std::unordered_map<unsigned int, std::string>& shaderSources)
	{
		std::vector<GLuint> shaderIDs;

		GLuint program = glCreateProgram();

		for (auto&& [key, value] : shaderSources)
		{
			GLenum type = key;
			const std::string& source = value;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				DVS_CORE_ERROR("{0}", &infoLog[0]);
				DVS_ASSERT(false, "Error compiling shader");
				break;
			}

			glAttachShader(program, shader);
			shaderIDs.emplace_back(shader);
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (GLuint id : shaderIDs)
				glDeleteShader(id);

			DVS_CORE_ERROR("{0}", &infoLog[0]);
			DVS_ASSERT(false, "Error linking shader!");
			return;
		}

		// Always detach shaders after a successful link.
		for (GLuint id : shaderIDs)
			glDetachShader(program, id);

		// Only set RenderID after successful shader compilation
		m_RenderID = program;
	}

	// --------------------------------------------------
	// --- UPLOAD UNIFORMS ---
	void OpenGLShader::_UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::_UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::_UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::_UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::_UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::_UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::_UploadUniformMat3(const std::string& name, const glm::mat3& value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::_UploadUniformMat4(const std::string& name, const glm::mat4& value)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

}
