#pragma once

#ifdef CAST_PLATFORM_WINDOWS

//#include <Debug/Log.h>
//#include <nfd.h>
#include "3DCast/Core/StartupBanner.h"

extern Cast::Application* Cast::CreateApplication();

int main(int argc, char** argv) {
	API::LogAPI::Init();

	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging");

	Cast::Core::StartupBanner banner;
	if (!banner.Init())
		return -1;

	banner.Blit(5000);

	NFD_Init();

	auto app = Cast::CreateApplication();
	app->Run();

	delete app;
	NFD_Quit();

	return 0;
}

#endif