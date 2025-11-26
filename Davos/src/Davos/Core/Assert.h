#pragma once
#include "Davos/Core/Core.h"
#include "Davos/Core/Log.h"

#include <filesystem>

#ifdef DVS_ENABLE_ASSERTS

	#define DVS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { DVS##type##ERROR(msg, __VA_ARGS__); DVS_DEBUGBREAK(); } }
	#define DVS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) DVS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define DVS_INTERNAL_ASSERT_NO_MSG(type, check) DVS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", DVS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define DVS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define DVS_INTERNAL_ASSERT_GET_MACRO(...) DVS_EXPAND_MACRO( DVS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, DVS_INTERNAL_ASSERT_WITH_MSG, DVS_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define DVS_ASSERT(...) DVS_EXPAND_MACRO( DVS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define DVS_CORE_ASSERT(...) DVS_EXPAND_MACRO( DVS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )

#else
	#define DVS_ASSERT(...)
	#define DVS_CORE_ASSERT(...)
#endif