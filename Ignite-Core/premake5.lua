project "Ignite-Core"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	VULKAN = os.getenv("VULKAN_SDK")
	
	pchheader "igpch.h"
	pchsource "src/igpch.cpp"


	files
	{
		"include/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"include",
		"$(VULKAN_SDK)/include",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLM}",
		"vendor/stb/include"
	}

	links 
	{ 
		"$(VULKAN_SDK)/lib/vulkan-1.lib",
		"GLFW"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IGCORE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_VULKAN"
		}

	filter "configurations:Debug"
		defines "IGCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "IGCORE_RELEASE"
		runtime "Release"
		optimize "on"
