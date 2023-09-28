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

include "3DCast/vendor/GLFW"
include "3DCast/vendor/GLEW"
include "3DCast/vendor/imgui"
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
		"%{prj.name}/OpenGL_util/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"GLWrapperLib",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.GLEW}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}"
	}

	links{
		"GLWrapperLib",
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
		"3DCast/vendor/spdlog/include",
		"3DCast/src",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}"
	}

	links{
		"3DCast"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"CAST_PLATFORM_WINDOWS",
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


project "GLWrapperLib"
	location "GLWrapperLib"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir (outputdirBIN)
	objdir (outputdirOBJ)

	files{
		"%{prj.name}/OpenGL_util/**.h",
		"%{prj.name}/OpenGL_util/**.cpp",
		"%{prj.name}/OpenGL_util/**.hpp"
	}

	includedirs{
		"%{prj.name}/dependencies/assimp-5.2.5/include",
		"%{prj.name}/dependencies/yaml-cpp/include",
		"%{IncludeDirs.GLEW}",
		"%{prj.name}/OpenGL_util",
		"%{prj.name}/OpenGL_util/vendor"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "CAST_DEBUG"
		symbols "On"
		runtime "Debug"

		links{
			"assimp-vc142-mtd.lib",
			"yaml-cppd.lib",
			"opengl32.lib",
			"GLEW"
		}

		libdirs{
			"%{prj.name}/dependencies/assimp-5.2.5/lib/Debug",
			"%{prj.name}/dependencies/yaml-cpp/lib/Debug"
		}

	filter {"configurations:Release", "configurations:Dist"}
		defines "CAST_RELEASE"
		optimize "On"
		buildoptions "/MD"

		links{
			"assimp-vc142-mt.lib",
			"yaml-cpp.lib",
			"opengl32.lib",
			"GLEW"
		}

		libdirs{
			"%{prj.name}/dependencies/assimp-5.2.5/lib/Release",
			"%{prj.name}/dependencies/yaml-cpp/lib/Release"
		}