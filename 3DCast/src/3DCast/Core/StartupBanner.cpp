#include "castpch.h"
#include "StartupBanner.h"

#include "3DCast/Platform/Linux/LinuxStartupBanner.h"

Cast::Scope<Cast::Core::StartupBanner> Cast::Core::StartupBanner::Create()
{
#ifdef CAST_PLATFORM_WINDOWS
	return CreateScope<WindowsStartupBanner>();
#elif CAST_PLATFORM_LINUX
	return CreateScope<LinuxStartupBanner>();
#else
	CAST_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}
