# Main executable - Entry point
file(GLOB_RECURSE RUNTIME_SOURCES
    "src/*.cpp"
    "src/*.c")

file(GLOB_RECURSE RUNTIME_HEADERS  
    "src/*.h"
    "src/*.hpp") 

add_executable(Runtime ${RUNTIME_SOURCES} ${RUNTIME_HEADERS})
target_compile_features(Runtime PUBLIC cxx_std_17)

create_source_groups(${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(Runtime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Runtime PUBLIC 
    3DCast
    glfw
    imgui
)
 
# Define Symbols
target_compile_definitions(Runtime PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>)