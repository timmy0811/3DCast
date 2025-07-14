# 3DCast static library

# Note: GLOB_RECURSE is convenient but doesn't automatically detect new/removed files
# For better build system behavior, consider listing files explicitly in larger projects
file(GLOB_RECURSE 3DCAST_SOURCES
        "src/*.cpp"
        "src/*.c")

file(GLOB_RECURSE 3DCAST_HEADERS
        "src/*.h"
        "src/*.hpp")

# Remove .vshistory files from the sources and headers
list(FILTER 3DCAST_SOURCES EXCLUDE REGEX ".*\.vshistory/.*")
list(FILTER 3DCAST_HEADERS EXCLUDE REGEX ".*\.vshistory/.*")

add_library(3DCast STATIC ${3DCAST_SOURCES} ${3DCAST_HEADERS})
add_library(3DCast::3DCast ALIAS 3DCast)  # Create namespaced alias for modern CMake usage

create_source_groups(${CMAKE_CURRENT_SOURCE_DIR})

find_package(TBB REQUIRED)
if(TBB_FOUND)
    message("Found TBB: ${TBB_VERSION}")
    target_link_libraries(3DCast PUBLIC TBB::tbb)

    target_compile_definitions(3DCast PUBLIC TBB)

else ()
    message("TBB not found, proceeding without TBB support.")
endif ()

# Separate public from private includes
target_include_directories(3DCast
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Vendor
        ${CMAKE_SOURCE_DIR}/Dependencies/imgui
        ${CMAKE_SOURCE_DIR}/Dependencies/glfw/include)

# Precompiled header
target_precompile_headers(3DCast PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/castpch.h)

target_compile_definitions(3DCast
        PUBLIC
        CAST_BUILD_STATIC
        GL_STATIC
        _CRT_SECURE_NO_WARNINGS

        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Debug>:CAST_DEBUG>
        $<$<CONFIG:Debug>:CAST_ENABLE_ASSERTS>

        $<$<CONFIG:Release>:RELEASE>
        $<$<CONFIG:Release>:CAST_RELEASE>
        $<$<CONFIG:Release>:NO_ASSERTS>
)

# Platform-specific settings
if (WIN32)
    target_compile_definitions(3DCast PUBLIC CAST_PLATFORM_WINDOWS)
elseif (UNIX AND NOT APPLE)
    target_compile_definitions(3DCast PUBLIC CAST_PLATFORM_LINUX)
elseif (APPLE)
    target_compile_definitions(3DCast PUBLIC CAST_PLATFORM_MACOS)
else ()
    message(FATAL_ERROR "Unsupported platform detected. The build cannot continue.")
endif ()

# Dependencies
target_link_libraries(3DCast
        PUBLIC
        GLWrapper
        nfd)
