workspace "Ignite-Sandbox"
	architecture "x64"
	startproject "Ignite-Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["IMGUI"] = "vendor/imgui"
IncludeDir["GLM"] = "vendor/glm"
VULKAN = os.getenv("VULKAN_SDK")

-- Projects
group "Dependencies"
	include "Ignite-Core/vendor/glfw.lua"
	include "Ignite-Core/vendor/imgui"
group ""

include "Ignite-Core"
include "Ignite-Sandbox"