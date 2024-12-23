# CMake Version
cmake_minimum_required(VERSION 3.7...3.12) 

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Global Properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Global Function Definitions
function(create_source_groups base_dir)
    file(GLOB_RECURSE ALL_FILES RELATIVE ${base_dir} ${base_dir}/*)
    foreach(FILE ${ALL_FILES})
        get_filename_component(PATH "${FILE}" PATH)
        string(REPLACE "/" "\\" GROUP "${PATH}")
        source_group("${GROUP}" FILES "${base_dir}/${FILE}")
    endforeach()
endfunction()

# Variables
set(DEV_DISTRIBUTED TRUE) # Set to TRUE if the project files should get distributed to the project dirs

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src) 

# Binary paths use VS makro!
message(NOTICE "Usage of IDE specific makro \"$(Platform)\"")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$(Platform)-$<CONFIG>/$<TARGET_PROPERTY:NAME>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$(Platform)-CONFIG>/$<TARGET_PROPERTY:NAME>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$(Platform)-$<CONFIG>/$<TARGET_PROPERTY:NAME>)
set(CMAKE_OBJECT_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin-int/$(Platform)-$<CONFIG>/$<TARGET_PROPERTY:NAME>)

# Project
project(3DCast VERSION 1.0
    DESCRIPTION "Rasterization and Ray tracing engine"
    LANGUAGES C CXX)
  
# Subdirectories
set(GENERATOR_PATH ${CMAKE_BINARY_DIR}) 

if(${DEV_DISTRIBUTED})
    set(GENERATOR_PATH ${CMAKE_SOURCE_DIR})
endif()  

add_subdirectory(3DCast_Runtime ${GENERATOR_PATH}/3DCast_Runtime)
add_subdirectory(3DCast ${GENERATOR_PATH}/3DCast)
add_subdirectory(GLWrapper ${GENERATOR_PATH}/GLWrapper)

# ---- Dependencies ----
# Assimp
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "" FORCE)

add_subdirectory(dependencies/assimp)

set_target_properties(assimp PROPERTIES FOLDER "Dependencies/assimp")
set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies/assimp")
set_target_properties(uninstall PROPERTIES FOLDER "Dependencies/assimp")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies/assimp")

# GLEW
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(BUILD_UTILS OFF CACHE INTERNAL "" FORCE)

add_subdirectory(dependencies/glew/build/cmake)

set_target_properties(glew PROPERTIES FOLDER "Dependencies/glew")
set_target_properties(glew_s PROPERTIES FOLDER "Dependencies/glew")

# YAML-CPP
set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE INTERNAL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)

add_subdirectory(dependencies/yaml-cpp)

set_target_properties("yaml-cpp" PROPERTIES FOLDER "Dependencies/yaml-cpp")

set_target_properties("Continuous" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")
set_target_properties("Experimental" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")
set_target_properties("Nightly" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")
set_target_properties("NightlyMemoryCheck" PROPERTIES FOLDER "Dependencies/yaml-cpp/tests")

# spdlog
add_subdirectory(dependencies/spdlog)

set_target_properties("spdlog" PROPERTIES FOLDER "Dependencies/spdlog")

# glm -> Use header only version GLWrapper/vendor/glm !!!
# add_subdirectory(dependencies/glm)

set(DGLM_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(DBUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)

# set_target_properties("glm" PROPERTIES FOLDER "Dependencies/glm")

# GLFW
add_subdirectory(Dependencies/glfw)
set_target_properties("glfw" PROPERTIES FOLDER "Dependencies/glfw")
set_target_properties("update_mappings" PROPERTIES FOLDER "Dependencies/glfw")

# ImGui
add_subdirectory(Dependencies/imgui)
set_target_properties("imgui" PROPERTIES FOLDER "Dependencies/imgui")

# Generator Settings
# Visual Studio Startup Project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Runtime)