project "GraphicsAPI"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir (outputdirBIN)
	objdir (outputdirOBJ)

	pchheader "glpch.h"
	pchsource "src/glpch.cpp"

	files{
		"src/**.h",
		"src/**.cpp",
		"src/**.hpp"
	}

	-- Fix and Remove!!! --
	removefiles { "src/API/renderer/SpriteRenderer.h", "src/API/renderer/SpriteRenderer.cpp" }
	-----------------------

	includedirs{
		"dependencies/assimp-5.2.5/include",
		"dependencies/yaml-cpp/include",
		"src",
		"../%{IncludeDirs.GLEW}",
		"../%{IncludeDirs.spdlog}"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"

		links{
			"assimp-vc142-mtd.lib",
			"yaml-cppd.lib",
			"opengl32.lib",
			"GLEW"
		}

		libdirs{
			"dependencies/assimp-5.2.5/lib/Debug",
			"dependencies/yaml-cpp/lib/Debug"
		}

	filter {"configurations:Release", "configurations:Dist"}
		optimize "On"
		runtime "Release"

		linkoptions {
			"/OPT:NOREF",  -- Disable removal of unreferenced COMDAT data
		}

		links{
			"assimp-vc142-mt.lib",
			"yaml-cpp.lib",
			"opengl32.lib",
			"GLEW"
		}

		libdirs{
			"dependencies/assimp-5.2.5/lib/Release",
			"dependencies/yaml-cpp/lib/Release"
		}