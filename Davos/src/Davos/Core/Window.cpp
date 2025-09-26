#include "dvs_pch.h"
#include "Window.h"

#ifdef DVS_PLATFORM_WINDOWS
	#include "Platform/Windows/Window_windows.h"
#endif

namespace Davos {

	Scope<Window> Window::Create(const WindowProperties& properties)
	{
	#ifdef DVS_PLATFORM_WINDOWS
		return CreateScope<WindowWindows>(properties);
	#else
		DVS_CORE_ASSERT(false, "Unable to create Window: unknown platform");
		return nullptr;
	#endif
	}

}
