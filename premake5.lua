include "Dependencies.lua"

workspace "Davos"
	architecture "x86_64"
	startproject "DavosEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "Davos/vendor/GLFW"
	include "Davos/vendor/Glad"
	include "Davos/vendor/ImGui"
	include "Davos/vendor/yaml-cpp"
group ""

group "Core"
	include "Davos"
group ""

group "Tools"
	include "DavosEditor"
group ""

group "Misc"
	include "LaserBeam"
group ""