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

include "3DCast/vendor/GLFW"
include "3DCast/vendor/GLEW"
include "3DCast/vendor/imgui"
----------------------------------------

project "3DCast"
	location "3DCast"
	kind "SharedLib"
	language "C++"

	targetdir (outputdirBIN)
	objdir (outputdirOBJ)

	pchheader "castpch.h"
	pchsource "3DCast/src/castpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/OpenGL_util/**.hpp"
	}

	includedirs{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"GLWrapperLib",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.GLEW}",
		"%{IncludeDirs.ImGui}"
	}

	links{
		"GLWrapperLib",
		"GLFW",
		"GLEW",
		"ImGui",
		"opengl32.lib"
	}

	postbuildcommands{
		"{COPY} $(SolutionDir)bin/$(Platform)-$(Configuration)/3DCast/3DCast.dll $(SolutionDir)bin/$(Platform)-$(Configuration)/3DCast_Runtime/"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"CAST_PLATFORM_WINDOWS",
			"CAST_BUILD_DLL",
			"GLEW_STATIC"
		}

	filter "configurations:Debug"
		defines {"CAST_DEBUG", "CAST_ENABLE_ASSERTS"}
		symbols "On"
		buildoptions "/MDd"

	filter "configurations:Release"
		defines "CAST_RELEASE"
		optimize "On"
		buildoptions "/MT"

	filter "configurations:Dist"
		defines "CAST_DIST"
		optimize "On"
		buildoptions "/MT"

project "3DCast_Runtime"
	location "3DCast_Runtime"
	kind "ConsoleApp"
	language "C++"

	targetdir (outputdirBIN)
	objdir (outputdirOBJ)

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/OpenGL_util/**.hpp"
	}

	includedirs{
		"3DCast/vendor/spdlog/include",
		"3DCast/src"
	}

	links{
		"3DCast"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"CAST_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "CAST_DEBUG"
		symbols "On"
		buildoptions "/MDd"


	filter "configurations:Release"
		defines "CAST_RELEASE"
		optimize "On"
		buildoptions "/MD"

	filter "configurations:Dist"
		defines "CAST_DIST"
		optimize "On"
		buildoptions "/MD"


project "GLWrapperLib"
	location "GLWrapperLib"
	kind "StaticLib"
	language "C++"

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
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		defines "CAST_DEBUG"
		symbols "On"
		buildoptions "/MDd"

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