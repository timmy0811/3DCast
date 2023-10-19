#pragma once

#ifdef CAST_PLATFORM_WINDOWS

#include <Debug/Log.h>

extern Cast::Application* Cast::CreatApplication();

int main(int argc, char** argv) {
	API::LogAPI::Init();

	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging");

	auto app = Cast::CreatApplication();
	app->Run();

	delete app;

	return 0;
}

#endif