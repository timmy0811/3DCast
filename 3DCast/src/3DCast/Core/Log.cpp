#include "castpch.h"
#include "Log.h"

#include <vector>
#include <memory>

// Define the static member variables.
std::shared_ptr<spdlog::logger> Cast::Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Cast::Log::s_ClientLogger;
std::shared_ptr<Cast::ImGuiSink_mt> Cast::Log::s_ImGuiSink;

void Cast::Log::Init()
{
	s_ImGuiSink = std::make_shared<ImGuiSink_mt>();
	s_ImGuiSink->set_pattern("%^[%T.%e] %n: %v%$");

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(s_ImGuiSink);

#ifdef CAST_DEBUG
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%T.%e] %n: %v%$");
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

	// Create the core logger with multiple sinks.
	s_CoreLogger = std::make_shared<spdlog::logger>("3DCast", begin(sinks), end(sinks));
	s_CoreLogger->set_level(spdlog::level::trace);
	spdlog::register_logger(s_CoreLogger);

	// Create the client logger similarly.
	s_ClientLogger = std::make_shared<spdlog::logger>("Runtime", begin(sinks), end(sinks));
	s_ClientLogger->set_level(spdlog::level::trace);
	spdlog::register_logger(s_ClientLogger);
}