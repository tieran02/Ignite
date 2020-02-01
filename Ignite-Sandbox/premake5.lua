project "Ignite-Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("../bin/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"../Ignite-Core/include",
		"../Ignite-Core/%{IncludeDir.GLM}"
	}

	links
	{
		"Ignite-Core"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IGCORE_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
		  ("{COPY} %{prj.location}/resources ../bin/" .. outputdir .. "/%{prj.name}/resources")
		}

	filter "configurations:Debug"
		defines "IGCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "IGCORE_RELEASE"
		runtime "Release"
        optimize "on"