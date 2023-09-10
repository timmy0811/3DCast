#pragma once

#include <string>
#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Cast {
	class CAST_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }

		inline static void SetCoreLogLevel(spdlog::level::level_enum level) { s_CoreLogger->set_level(level); }
		inline static void SetClientLogLevel(spdlog::level::level_enum level) { s_ClientLogger->set_level(level); }

		inline static void SetCoreLogPattern(const std::string& pattern) { s_CoreLogger->set_pattern(pattern); }
		inline static void SetClientLogPattern(const std::string& pattern) { s_ClientLogger->set_pattern(pattern); }

		inline static void FlushLogsPeriodically(unsigned int seconds) { spdlog::flush_every(std::chrono::seconds(seconds)); }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define LOG_CORE_FATAL(...) ::Cast::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Cast::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::Cast::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::Cast::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_TRACE(...) ::Cast::Log::GetCoreLogger()->trace(__VA_ARGS__)

#define LOG_FATAL(...) ::Cast::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define LOG_ERROR(...) ::Cast::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Cast::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::Cast::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::Cast::Log::GetClientLogger()->trace(__VA_ARGS__)

#define LOG_START_LOCAL_SW(...) spdlog::stopwatch(__VA_ARGS__)
#define LOG_SW_ELAPSED(...) spdlog::debug("Elapsed {}", __VA_ARGS__)