#pragma once

// This file includes dependencies of overlaying projects into the wrapper vendor
// Change this according to your linker settings
#include <GLEW/glew.h>
#include <yaml-cpp/yaml.h>

#define _AMD64_
#define NOMINMAX

#define INCLUDE_ASSIMP

#ifdef INCLUDE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

// Defines
// #define FILTER_SHADOW