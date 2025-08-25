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
