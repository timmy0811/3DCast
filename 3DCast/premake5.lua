workspace "3DCast"
	architecture "x64"
	startproject "3DCast_Runtime"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdirBIN = "$(SolutionDir)bin/$(Platform)-$(Configuration)/$(ProjectName)/"
outputdirOBJ = "$(SolutionDir)bin-int/$(Platform)-$(Configuration)/$(ProjectName)/"

-- Includes ----------------------------
IncludeDirs = {}
IncludeDirs["GLFW"] = "3DCast/vendor/GLFW/include"
IncludeDirs["GLEW"] = "3DCast/vendor/GLEW/include"
IncludeDirs["ImGui"] = "3DCast/vendor/imgui"
IncludeDirs["glm"] = "3DCast/vendor/glm"
IncludeDirs["spdlog"] = "3DCast/vendor/spdlog/include"
IncludeDirs["GraphicsAPI"] = "GraphicsAPI/src"

include "3DCast/vendor/GLFW"
include "3DCast/vendor/GLEW"
include "3DCast/vendor/imgui"
include "GraphicsAPI"
----------------------------------------

project "3DCast"
	location "3DCast"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir (outputdirBIN)
	objdir (outputdirOBJ)

	pchheader "castpch.h"
	pchsource "3DCast/src/castpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs{
		"%{prj.name}/src",

		"%{IncludeDirs.GraphicsAPI}",
		"GraphicsAPI/dependencies/assimp-5.2.5/include",
		"GraphicsAPI/dependencies/yaml-cpp/include",

		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.GLEW}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.spdlog}",
	}

	links{
		"GraphicsAPI",
		"GLFW",
		"GLEW",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"CAST_PLATFORM_WINDOWS",
			"CAST_BUILD_STATIC",
			"GLEW_STATIC",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		defines {"CAST_DEBUG", "CAST_ENABLE_ASSERTS"}
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines "CAST_RELEASE"
		optimize "On"
		runtime "Release"

	filter "configurations:Dist"
		defines "CAST_DIST"
		optimize "On"
		runtime "Release"

project "3DCast_Runtime"
	location "3DCast_Runtime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir (outputdirBIN)
	objdir (outputdirOBJ)

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/OpenGL_util/**.hpp"
	}

	includedirs{
		"%{IncludeDirs.GraphicsAPI}",
		"GraphicsAPI/dependencies/assimp-5.2.5/include",
		"GraphicsAPI/dependencies/yaml-cpp/include",

		"3DCast/vendor/spdlog/include",
		"3DCast/src",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.GLEW}"
	}

	links{
		"3DCast",
		"GraphicsAPI"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"CAST_PLATFORM_WINDOWS",
		}

		linkoptions {
			"/ignore:4217"
		}

	filter "configurations:Debug"
		defines "CAST_DEBUG"
		symbols "On"
		runtime "Debug"


	filter "configurations:Release"
		defines "CAST_RELEASE"
		optimize "On"
		runtime "Release"

	filter "configurations:Dist"
		defines "CAST_DIST"
		optimize "On"
		buildoptions "/MD"