#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Davos {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

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
