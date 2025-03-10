#pragma once

#ifdef CAST_PLATFORM_WINDOWS

#include <Debug/Log.h>
#include <nfd.h>

extern Cast::Application* Cast::CreateApplication();

int main(int argc, char** argv) {
	API::LogAPI::Init();

	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging");
	NFD_Init();

	auto app = Cast::CreateApplication();
	app->Run();

	delete app;
	NFD_Quit();

	return 0;
}

#endif