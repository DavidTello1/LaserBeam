#pragma once
#include "Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Davos {

	class Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core Log Macros
#define DVS_CORE_TRACE(...)		::Davos::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DVS_CORE_INFO(...)		::Davos::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DVS_CORE_WARN(...)		::Davos::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DVS_CORE_ERROR(...)		::Davos::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DVS_CORE_CRITICAL(...)	::Davos::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros
#define DVS_TRACE(...)			::Davos::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DVS_INFO(...)			::Davos::Log::GetClientLogger()->info(__VA_ARGS__)
#define DVS_WARN(...)			::Davos::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DVS_ERROR(...)			::Davos::Log::GetClientLogger()->error(__VA_ARGS__)
#define DVS_CRITICAL(...)		::Davos::Log::GetClientLogger()->critical(__VA_ARGS__)
