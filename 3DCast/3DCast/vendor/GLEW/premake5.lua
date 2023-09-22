project "GLEW"
    kind "StaticLib"
    language "C"
    
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
        buildoptions { "-std=c11", "-lgdi32" }
        systemversion "latest"
        staticruntime "On"

    filter "configurations:Debug"
		defines {
            "WIN32", 
            "_WINDOWS",
            "GLEW_STATIC",
            "VC_EXTRALEAN",
            "GLEW_NO_GLU",
            "CMAKE_INTDIR=\"Debug\""
        }

		symbols "On"

    filter { "system:windows", "configurations:Release", "configurations:Dist" }
        buildoptions "/MT"
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