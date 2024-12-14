# Socket static library
file(GLOB_RECURSE 3DCAST_SOURCES
    "src/*.cpp"
    "src/*.c")  

file(GLOB_RECURSE 3DCAST_HEADERS
    "src/*.h"
    "src/*.hpp") 

add_library(3DCast STATIC ${3DCAST_SOURCES} ${3DCAST_HEADERS})
  
create_source_groups(${CMAKE_CURRENT_SOURCE_DIR}) 

target_include_directories(3DCast PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vendor
    ${CMAKE_SOURCE_DIR}/Dependencies/imgui
    ${CMAKE_SOURCE_DIR}/Dependencies/glfw/include)

target_compile_features(3DCast PUBLIC cxx_std_17)
target_precompile_headers(3DCast PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/castpch.h)

target_compile_definitions(3DCast PUBLIC
    CAST_PLATFORM_WINDOWS
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

target_link_libraries(3DCast PUBLIC GLWrapper)
