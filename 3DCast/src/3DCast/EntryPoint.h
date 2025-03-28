#pragma once

#ifdef CAST_PLATFORM_WINDOWS

#include "3DCast/Core/StartupBanner.h"

extern Cast::Application* Cast::CreateApplication();

int main(int argc, char** argv) {
#ifdef CAST_RELEASE
	FreeConsole();
#endif

	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging for 3DCast");

	API::LogAPI::Init(Cast::Log::GetImGuiSink());
	API::LogAPI::GetCoreLogger()->info("Initialized logging for GLWrapper");

#ifdef CAST_RELEASE
	Cast::Scope<Cast::Core::StartupBanner> banner = Cast::Core::StartupBanner::Create();
	if (!banner->Init("../3DCast/ressources/img/startup.png"))
		return -1;

	banner->Blit(4000);
#endif

	NFD_Init();

	auto app = Cast::CreateApplication();
	app->Run();

	delete app;
	NFD_Quit();
	spdlog::shutdown();

	return 0;
}

#endif