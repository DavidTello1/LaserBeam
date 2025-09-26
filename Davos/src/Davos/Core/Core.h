#pragma once
#include "Log.h"
#include "Assert.h"
#include <memory>

#ifdef DVS_PLATFORM_WINDOWS

#else
	#error Davos only supports Windows!
#endif

#ifdef DVS_DEBUG
	#define DVS_ENABLE_ASSERTS
	#if defined(DVS_PLATFORM_WINDOWS)
		#define DVS_DEBUGBREAK() __debugbreak()
	#elif defined(DVS_PLATFORM_LINUX)
		#include <signal.h>
		#define DVS_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet"
	#endif
#else
	#define DVS_DEBUGBREAK()
#endif

#define BIT(x) (1 << x)

#define DVS_BIND_EVENT_FN(fn)	std::bind(&fn, this, std::placeholders::_1)
//#define DVS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
//#define DVS_BIND_EVENT_FN(fn, ...) std::bind(&fn, this, __VA_ARGS__)

namespace Davos {

	template <typename T>
	using Scope = std::unique_ptr<T>;
	template <typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
