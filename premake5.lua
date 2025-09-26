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
IncludeDir["glm"] = "Davos/vendor/glm"
IncludeDir["stb"] = "Davos/vendor/stb"

group "Dependencies"
	include "Davos/vendor/GLFW"
	include "Davos/vendor/Glad"
	include "Davos/vendor/ImGui"

group ""

project "Davos"
	location "Davos"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "dvs_pch.h"
	pchsource "Davos/src/dvs_pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"Davos/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions "/utf-8"

		defines
		{
			"DVS_PLATFORM_WINDOWS"
		}
	
	filter "configurations:Debug"
		defines "DVS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "DVS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "DVS_DIST"
		runtime "Release"
		optimize "on"


project "LaserBeam"
	location "LaserBeam"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"Davos/vendor",
		"Davos/vendor/spdlog/include",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Davos"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions "/utf-8"

		defines
		{
			"DVS_PLATFORM_WINDOWS"
		}
	
	filter "configurations:Debug"
		defines "DVS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "DVS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "DVS_DIST"
		runtime "Release"
		optimize "on"
