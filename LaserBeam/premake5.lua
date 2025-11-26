project "LaserBeam"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Davos/vendor/spdlog/include",
        "%{wks.location}/Davos/src",
		"%{wks.location}/Davos/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Davos"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions "/utf-8"

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