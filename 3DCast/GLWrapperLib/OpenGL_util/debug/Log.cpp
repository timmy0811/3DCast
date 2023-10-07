#include "glpch.h"

#include "Log.h"

std::shared_ptr<spdlog::logger> GL::LogGL::s_CoreLogger;

void GL::LogGL::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_CoreLogger = spdlog::stdout_color_mt("GL_API");
	s_CoreLogger->set_level(spdlog::level::trace);
}