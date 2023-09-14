#pragma once

#ifdef CAST_PLATFORM_WINDOWS

extern Cast::Application* Cast::CreatApplication();

int main(int argc, char** argv) {
	Cast::Log::Init();
	Cast::Log::GetClientLogger()->info("Initialized Logging");

	auto app = Cast::CreatApplication();
	app->Run();

	delete app;

	return 0;
}

#endif