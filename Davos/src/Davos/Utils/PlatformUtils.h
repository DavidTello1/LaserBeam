#pragma once
#include <string>

namespace Davos {

	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter); // return empty string if cancelled
		static std::string SaveFile(const char* filter); // return empty string if cancelled
	};

	class Time
	{
	public:
		static float GetTime();
	};

}