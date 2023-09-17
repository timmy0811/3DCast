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

include "3DCast/vendor/GLFW"
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
		"%{IncludeDirs.GLFW}"
	}

	links{
		"GLWrapperLib",
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"CAST_PLATFORM_WINDOWS",
			"CAST_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines {"CAST_DEBUG", "CAST_ENABLE_ASSERTS"}
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

	postbuildcommands{
		"{COPY} $(SolutionDir)bin/$(Platform)-$(Configuration)/3DCast/3DCast.dll $(SolutionDir)bin/$(Platform)-$(Configuration)/$(ProjectName)/"
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
		"%{prj.name}/dependencies/glew-2.1.0/include",
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
			"glew32s.lib",
			"yaml-cppd.lib",
			"opengl32.lib"
		}

		libdirs{
			"%{prj.name}/dependencies/assimp-5.2.5/lib/Debug",
			"%{prj.name}/dependencies/yaml-cpp/lib/Debug",
			"%{prj.name}/dependencies/glew-2.1.0/lib/Release/x64",
		}

	filter {"configurations:Release", "configurations:Dist"}
		defines "CAST_RELEASE"
		optimize "On"
		buildoptions "/MD"

		links{
			"assimp-vc142-mt.lib",
			"glew32s.lib",
			"yaml-cpp.lib",
			"opengl32.lib"
		}

		libdirs{
			"%{prj.name}/dependencies/assimp-5.2.5/lib/Release",
			"%{prj.name}/dependencies/yaml-cpp/lib/Release",
			"%{prj.name}/dependencies/glew-2.1.0/lib/Release/x64",
		}