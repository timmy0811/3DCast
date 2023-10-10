#pragma once

#ifdef CAST_PLATFORM_WINDOWS

#include <OpenGL_util/debug/Log.h>

extern Cast::Application* Cast::CreatApplication();

int main(int argc, char** argv) {
	GL::LogGL::Init();

	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging");

	auto app = Cast::CreatApplication();
	app->Run();

	delete app;

	return 0;
}

#endif