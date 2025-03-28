#include "castpch.h"
#include "StartupBanner.h"

#include "3DCast/Platform/Windows/WindowsStartupBanner.h"

Cast::Scope<Cast::Core::StartupBanner> Cast::Core::StartupBanner::Create()
{
#ifdef CAST_PLATFORM_WINDOWS
	return CreateScope<Cast::Core::WindowsStartupBanner>();
#else
	CAST_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}