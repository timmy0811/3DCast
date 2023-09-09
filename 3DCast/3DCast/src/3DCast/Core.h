#pragma once

#ifdef CAST_PLATFORM_WINDOWS
#ifdef CAST_BUILD_DLL
#define CAST_API __declspec(dllexport)
#else
#define CAST_API __declspec(dllimport)
#endif
#else
#error 3DCast only supports Windows!
#endif