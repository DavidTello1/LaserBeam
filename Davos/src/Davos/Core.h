#pragma once

#ifdef DVS_PLATFORM_WINDOWS
	#ifdef DVS_BUILD_DLL
		#define DAVOS_API __declspec(dllexport)
	#else
		#define DAVOS_API __declspec(dllimport)
	#endif
#else
	#error Davos only supports Windows!
#endif

#ifdef DVS_DEBUG
	#define DVS_ENABLE_ASSERTS
#endif

#ifdef DVS_ENABLE_ASSERTS
	#define DVS_ASSERT(x, ...) { if(!(x)) { DVS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define DVS_CORE_ASSERT(x, ...) { if(!(x)) { DVS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();} }
#else
	#define DVS_ASSERT(x, ...)
	#define DVS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define DVS_BIND_EVENT_FN(fn)	std::bind(&fn, this, std::placeholders::_1)
//#define DVS_BIND_EVENT_FN(fn, ...) std::bind(&fn, this, __VA_ARGS__)
