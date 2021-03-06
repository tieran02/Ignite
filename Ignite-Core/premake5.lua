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
		"vendor/tinyobjloader/tiny_obj_loader.h",
		"vendor/mikktspace/mikktspace.h",
		"vendor/mikktspace/mikktspace.c"
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
		"%{IncludeDir.IMGUI}",
		"vendor/stb/include",
		"vendor/tinyobjloader",
		"vendor/mikktspace"
	}

	links 
	{ 
		"$(VULKAN_SDK)/lib/vulkan-1.lib",
		"GLFW",
		"ImGui"
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
		
	filter "files:**.c"
		flags {"NoPCH"}
