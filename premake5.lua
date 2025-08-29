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

project "Davos"
	location "Davos"
	kind "SharedLib"
	language "C++"

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
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		buildoptions "/utf-8"

		defines
		{
			"DVS_PLATFORM_WINDOWS",
			"DVS_BUILD_DLL"
		}

		postbuildcommands
		{
			("{MKDIR} ../bin/" .. outputdir .. "/LaserBeam"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/LaserBeam")
		}
	
	filter "configurations:Debug"
		defines "DVS_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "DVS_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "DVS_DIST"
		optimize "On"


project "LaserBeam"
	location "LaserBeam"
	kind "ConsoleApp"
	language "C++"

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
		staticruntime "On"
		systemversion "latest"
		buildoptions "/utf-8"

		defines
		{
			"DVS_PLATFORM_WINDOWS"
		}
	
	filter "configurations:Debug"
		defines "DVS_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "DVS_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "DVS_DIST"
		optimize "On"
