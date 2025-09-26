#pragma once
#include "Core.h"
#include"Log.h"

#ifdef DVS_ENABLE_ASSERTS
	#define DVS_ASSERT(x, ...) { if(!(x)) { DVS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define DVS_CORE_ASSERT(x, ...) { if(!(x)) { DVS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();} }
#else
	#define DVS_ASSERT(x, ...)
	#define DVS_CORE_ASSERT(x, ...)
#endif
