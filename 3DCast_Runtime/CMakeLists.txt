# Main executable - Entry point

file(GLOB_RECURSE RUNTIME_SOURCES
        "src/*.cpp"
        "src/*.c")

file(GLOB_RECURSE RUNTIME_HEADERS  
        "src/*.h"
        "src/*.hpp")

# Filter out version control history files
list(FILTER RUNTIME_SOURCES EXCLUDE REGEX ".*\.vshistory/.*")
list(FILTER RUNTIME_HEADERS EXCLUDE REGEX ".*\.vshistory/.*")

# Create executable with a more descriptive name
add_executable(Runtime ${RUNTIME_SOURCES} ${RUNTIME_HEADERS})
set_target_properties(Runtime PROPERTIES
        OUTPUT_NAME "3DCast_Runtime"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Set up source groups for IDE organization
create_source_groups(${CMAKE_CURRENT_SOURCE_DIR})

# Include directories
target_include_directories(Runtime
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link dependencies
target_link_libraries(Runtime
        PRIVATE
        3DCast
        glfw
        imgui)

# Define compiler features
# (C++17 is already set in the main CMakeLists.txt)
target_compile_features(Runtime PRIVATE cxx_std_17)

# Define compilation symbols
target_compile_definitions(Runtime
        PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:RELEASE_BUILD>
        $<$<CONFIG:Release>:NDEBUG>)

# Platform-specific settings
if (WIN32)
    # Windows-specific settings if needed
    target_compile_definitions(Runtime PRIVATE PLATFORM_WINDOWS)
elseif (UNIX AND NOT APPLE)
    # Linux-specific settings
    target_compile_definitions(Runtime PRIVATE PLATFORM_LINUX)

    if (WAYLAND)
        target_compile_definitions(Runtime PRIVATE USE_WAYLAND)
    endif ()
elseif (APPLE)
    # MacOS-specific settings
    target_compile_definitions(Runtime PRIVATE PLATFORM_MACOS)
endif ()

# Install rules
install(TARGETS Runtime
        RUNTIME DESTINATION bin)
