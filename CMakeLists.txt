# CMake Version
cmake_minimum_required(VERSION 3.14...3.22)

if (${CMAKE_VERSION} VERSION_LESS 3.22)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

# Set project info early
project(3DCast
        VERSION 1.0
        DESCRIPTION "Rasterization and Ray-tracing engine"
        LANGUAGES C CXX)

# Global settings
cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(VISUAL_STUDIO "Generate project files for Visual Studio" OFF)
option(WAYLAND "Build for Wayland instead of X11" OFF)

# Detect Wayland session
if (UNIX AND NOT APPLE)
    execute_process(
            COMMAND bash -c "echo $XDG_SESSION_TYPE"
            OUTPUT_VARIABLE SESSION_TYPE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (SESSION_TYPE STREQUAL "wayland" OR WAYLAND)
        message(STATUS "Wayland desktop detected or manually enabled")
        add_compile_definitions(CAST_DESKTOP_WAYLAND)
        set(USING_WAYLAND TRUE)
    else ()
        message(STATUS "Not using Wayland: XDG_SESSION_TYPE=${SESSION_TYPE}")
        set(USING_WAYLAND FALSE)
    endif ()
endif ()

# Global Function Definitions
function(create_source_groups base_dir)
    file(GLOB_RECURSE ALL_FILES RELATIVE ${base_dir} ${base_dir}/*)
    foreach (FILE ${ALL_FILES})
        get_filename_component(PATH "${FILE}" PATH)
        string(REPLACE "/" "\\" GROUP "${PATH}")
        source_group("${GROUP}" FILES "${base_dir}/${FILE}")
    endforeach ()
endfunction()

# ------------------------------------------------------------------------------
# Output directories (archives, libraries, executables, object files)

# If we're using a single-configuration generator (e.g. Unix Makefiles, Ninja):
if (NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
else ()
    # Multi-configuration generators (e.g. Visual Studio)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)

    if (VISUAL_STUDIO)
        message(STATUS "Setting Visual Studio startup project to 'Runtime'")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Runtime)
    endif ()
endif ()

set(ASSETS_PATH_DEV "${CMAKE_SOURCE_DIR}/3DCast/resources/")
set(ASSETS_PATH_DEPLOY "./resources/")

add_compile_definitions(
        $<$<CONFIG:Debug>:ASSET_DIR="${ASSETS_PATH_DEV}">
        $<$<CONFIG:Release>:ASSET_DIR="${ASSETS_PATH_DEPLOY}">
)

# ------------------------------------------------------------------------------
# Subdirectories
set(GENERATOR_PATH ${CMAKE_BINARY_DIR})
if (DEV_DISTRIBUTED)
    set(GENERATOR_PATH ${CMAKE_SOURCE_DIR})
endif ()

add_subdirectory(3DCast_Runtime ${GENERATOR_PATH}/3DCast_Runtime)
add_subdirectory(3DCast ${GENERATOR_PATH}/3DCast)
add_subdirectory(GLWrapper ${GENERATOR_PATH}/GLWrapper)

# ------------------------------------------------------------------------------
# ---- Dependencies ----

# Define dependency configuration options
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Allow options to be set with variables
set(BUILD_SHARED_LIBS OFF)

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Don't build assimp tests")
set(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable assimp export functionality")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Don't build assimp tools")
add_subdirectory(Dependencies/assimp)
set_target_properties(assimp PROPERTIES FOLDER "Dependencies/assimp")
if (WIN32)
    set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies/assimp")
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies/assimp")
endif ()
set_target_properties(uninstall PROPERTIES FOLDER "Dependencies/assimp")

# GLEW
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(BUILD_UTILS OFF CACHE BOOL "Don't build GLEW utilities")
if (USING_WAYLAND)
    set(GLEW_EGL ON CACHE BOOL "Use EGL with GLEW for Wayland")
    set(BUILD_UTILS ON CACHE BOOL "Build GLEW utilities for Wayland")
endif ()
add_subdirectory(Dependencies/glew/build/cmake)
set_target_properties(glew PROPERTIES FOLDER "Dependencies/glew")
set_target_properties(glew_s PROPERTIES FOLDER "Dependencies/glew")

# YAML-CPP
set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "Don't install yaml-cpp")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Don't build yaml-cpp tools")
add_subdirectory(Dependencies/yaml-cpp)
set_target_properties("yaml-cpp" PROPERTIES FOLDER "Dependencies/yaml-cpp")
set_target_properties("Continuous" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")
set_target_properties("Experimental" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")
set_target_properties("Nightly" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")
set_target_properties("NightlyMemoryCheck" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")

# spdlog
add_subdirectory(Dependencies/spdlog)
set_target_properties("spdlog" PROPERTIES FOLDER "Dependencies/spdlog")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW docs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples")
set(GLFW_INSTALL OFF CACHE BOOL "Don't install GLFW")
add_subdirectory(Dependencies/glfw)
set_target_properties("glfw" PROPERTIES FOLDER "Dependencies/glfw")
set_target_properties("update_mappings" PROPERTIES FOLDER "Dependencies/glfw")

# ImGui
add_subdirectory(Dependencies/imgui)
set_target_properties("imgui" PROPERTIES FOLDER "Dependencies/imgui")

# Native File Dialog (nfd)
add_subdirectory(Dependencies/nativefile)
# Note: GLM is header‐only; if you use the vendor copy in GLWrapper, no add_subdirectory is needed.
# set(DGLM_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
# set(DBUILD_SHARED_LIBS  OFF CACHE INTERNAL "" FORCE)
# add_subdirectory(Dependencies/glm)
# set_target_properties("glm" PROPERTIES FOLDER "Dependencies/glm")

# Note regarding GLM:
# GLM is header-only; if using the vendor copy in GLWrapper, no add_subdirectory is needed.
