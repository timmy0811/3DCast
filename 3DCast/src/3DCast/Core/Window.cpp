#include "castpch.h"
#include "3DCast/Core/Window.h"

#ifdef CAST_PLATFORM_WINDOWS
#include "3DCast/Platform/Windows/WindowsWindow.h"
#elif CAST_PLATFORM_LINUX
#include "3DCast/Platform/Linux/LinuxWindow.h"
#endif

namespace Cast
{
	Scope<Window> Window::Create(const WindowProperties& props)
	{
#ifdef CAST_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#elif CAST_PLATFORM_LINUX
		return CreateScope<LinuxWindow>(props);
#else
		CAST_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
