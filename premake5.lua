workspace "Ignite"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLM"] = "vendor/glm"
IncludeDir["SPDLOG"] = "vendor/spdlog/include"


VULKAN = os.getenv("VULKAN_SDK")


project "IgniteRenderer"
	location "%{wks.name}/%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	pchheader "igpch.h"
	pchsource "%{wks.name}/%{prj.name}/src/igpch.cpp"

	files
	{
		"%{wks.name}/%{prj.name}/include/**.h",
		"%{wks.name}/%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{wks.name}/%{prj.name}/include",
		"%{IncludeDir.SPDLOG}",
		"%{IncludeDir.GLM}"
	}

	links
	{

	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IG_PLATFROM_WINDOWS",
			"IG_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines
		{
			"IG_DEBUG",
			"IG_ENABLE_ASSERTS"
		}
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "IG_RELEASE"
		optimize "on"
		runtime "Release"
		
project "Sandbox"
	location "%{wks.name}/%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{wks.name}/%{prj.name}/include/**.h",
		"%{wks.name}/%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{wks.name}/IgniteRenderer/include",
		"%{IncludeDir.SPDLOG}",
		"%{IncludeDir.GLM}"
	}

	links
	{
		"IgniteRenderer"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IG_PLATFROM_WINDOWS"
		}

	filter "configurations:Debug"
		defines
		{
			"IG_DEBUG",
			"IG_ENABLE_ASSERTS"
		}
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "IG_RELEASE"
		optimize "on"
		runtime "Release"