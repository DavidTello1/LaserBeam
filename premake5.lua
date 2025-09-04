workspace "Davos"
	architecture "x64"
	startproject "LaserBeam"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Davos/vendor/GLFW/include"
IncludeDir["Glad"] = "Davos/vendor/Glad/include"
IncludeDir["ImGui"] = "Davos/vendor/ImGui"

group "Dependencies"
	include "Davos/vendor/GLFW"
	include "Davos/vendor/Glad"
	include "Davos/vendor/ImGui"

group ""

project "Davos"
	location "Davos"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "dvs_pch.h"
	pchsource "Davos/src/dvs_pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Davos/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		buildoptions "/utf-8"

		defines
		{
			"DVS_PLATFORM_WINDOWS",
			"DVS_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{MKDIR} ../bin/" .. outputdir .. "/LaserBeam"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/LaserBeam")
		}
	
	filter "configurations:Debug"
		defines "DVS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "DVS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "DVS_DIST"
		runtime "Release"
		optimize "On"


project "LaserBeam"
	location "LaserBeam"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Davos/src",
		"Davos/vendor/spdlog/include"
	}

	links
	{
		"Davos"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		buildoptions "/utf-8"

		defines
		{
			"DVS_PLATFORM_WINDOWS"
		}
	
	filter "configurations:Debug"
		defines "DVS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "DVS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "DVS_DIST"
		runtime "Release"
		optimize "On"
