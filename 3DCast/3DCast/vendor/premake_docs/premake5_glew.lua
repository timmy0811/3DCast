project "GLEW"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    
    targetdir (outputdirBIN)
    objdir (outputdirOBJ)

    files{
        "include/GLEW/glew.h",
        "include/GLEW/glxew.h",
        "include/GLEW/wglew.h",
        "src/glew.c",
        "build/glew.rc"
    }

    includedirs{
        "include"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"

		defines {
            "WIN32", 
            "_WINDOWS",
            "GLEW_STATIC",
            "VC_EXTRALEAN",
            "GLEW_NO_GLU",
            "CMAKE_INTDIR=\"Debug\""
        }

		symbols "On"

    filter { "configurations:Release", "configurations:Dist" }
        runtime "Release"
        optimize "ON"

        defines {
            "WIN32", 
            "_WINDOWS",
            "GLEW_STATIC",
            "VC_EXTRALEAN",
            "GLEW_NO_GLU",
            "CMAKE_INTDIR=\"Release\"",
            "NDEBUG"
        }