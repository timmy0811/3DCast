#pragma once

extern Cast::Application* Cast::CreateApplication();

int main(int argc, char** argv)
{
#ifdef CAST_RELEASE
	FreeConsole();
#endif

	// #ifdef CAST_PLATFORM_LINUX
	// 	gtk_disable_setlocale();
	// #endif

	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging for 3DCast");

	API::LogAPI::Init(Cast::Log::GetImGuiSink());
	API::LogAPI::GetCoreLogger()->info("Initialized logging for GLWrapper");

#ifdef CAST_RELEASE
	Cast::Scope<Cast::Core::StartupBanner> banner = Cast::Core::StartupBanner::Create();
	if (!banner->Init(std::string(ASSET_DIR) + "img/startup.png"))
		return -1;

	banner->Blit(4000);
#endif

	NFD_Init();

	const auto app = Cast::CreateApplication();
	app->Run();

	delete app;
	NFD_Quit();
	spdlog::shutdown();

	return 0;
}
