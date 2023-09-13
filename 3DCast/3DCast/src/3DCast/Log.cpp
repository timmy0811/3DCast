#include "castpch.h"

#include "Log.h"

std::shared_ptr<spdlog::logger> Cast::Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Cast::Log::s_ClientLogger;

void Cast::Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_CoreLogger = spdlog::stdout_color_mt("3DCast");
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger = spdlog::stdout_color_mt("Runtime");
	s_ClientLogger->set_level(spdlog::level::trace);
}