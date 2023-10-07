#pragma once

#ifdef CAST_PLATFORM_WINDOWS
// Other defines
#else
#error 3DCast only supports Windows!
#endif

#ifdef CAST_ENABLE_ASSERTS
#define CAST_ASSERT(x, ...) {if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CAST_CORE_ASSERT(x, ...) {if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define CAST_ASSERT(x, ...) {if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CAST_CORE_ASSERT(x, ...) {if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#endif

#define BIT(x) (1 << x)

#define CAST_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)