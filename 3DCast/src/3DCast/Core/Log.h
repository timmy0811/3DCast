#pragma once

#include "3DCast/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

#include <mutex>
#include <vector>
#include <string>

namespace Cast {
	template<typename Mutex>
	class ImGuiSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		const std::vector<std::string>& GetLog() const { return m_LogBuffer; }
		void Clear() { m_LogBuffer.clear(); }

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override {
			spdlog::memory_buf_t formatted;
			this->formatter_->format(msg, formatted);
			m_LogBuffer.push_back(fmt::to_string(formatted));
		}

		void flush_() override { /* No flushing required for an in-memory sink */ }

	private:
		std::vector<std::string> m_LogBuffer;
	};

	using ImGuiSink_mt = ImGuiSink<std::mutex>;

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }

		inline static std::shared_ptr<ImGuiSink_mt> GetImGuiSink() { return s_ImGuiSink; }

		inline static void SetCoreLogLevel(spdlog::level::level_enum level) { s_CoreLogger->set_level(level); }
		inline static void SetClientLogLevel(spdlog::level::level_enum level) { s_ClientLogger->set_level(level); }

		inline static void SetCoreLogPattern(const std::string& pattern) { s_CoreLogger->set_pattern(pattern); }
		inline static void SetClientLogPattern(const std::string& pattern) { s_ClientLogger->set_pattern(pattern); }

		inline static void FlushLogsPeriodically(unsigned int seconds) { spdlog::flush_every(std::chrono::seconds(seconds)); }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

		static std::shared_ptr<ImGuiSink_mt> s_ImGuiSink;
	};
}

#define LOG_CORE_FATAL(...) ::Cast::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Cast::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_WARN(...)  ::Cast::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_INFO(...)  ::Cast::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_TRACE(...) ::Cast::Log::GetCoreLogger()->trace(__VA_ARGS__)

#define LOG_FATAL(...) ::Cast::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define LOG_ERROR(...) ::Cast::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)  ::Cast::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)  ::Cast::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::Cast::Log::GetClientLogger()->trace(__VA_ARGS__)

#define LOG_START_LOCAL_SW(name) spdlog::stopwatch name
#define LOG_SW_ELAPSED(name) spdlog::debug("Elapsed {0}", name)