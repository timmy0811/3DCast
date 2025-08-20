#pragma once

#include <cstdint>
#include <functional>

namespace Cast
{
	typedef uint32_t uid;

	class UID
	{
	public:
		UID(const uint32_t value = 0) : m_ID(value) {}
		~UID() = default;

		operator uid() const { return m_ID; }

		bool operator==(const UID& other) const { return m_ID == other.m_ID; }
		bool operator!=(const UID& other) const { return m_ID != other.m_ID; }
		bool operator<(const UID& other) const { return m_ID < other.m_ID; }

		inline static UID Create() { return GlobalId++; }
		inline static UID None() { return 0; }

		inline uid GetID() const { return m_ID; }

	private:
		uid m_ID;
		static inline uint32_t GlobalId = 0x100;
	};
}

template<>
struct std::hash<Cast::UID>
{
	size_t operator()(const Cast::UID& uid) const noexcept
	{
		return hash<Cast::uid>()(uid.GetID());
	}
};

#include <fmt/format.h>

template <>
struct fmt::formatter<Cast::UID> {
	static constexpr auto parse(const format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Cast::UID& uid, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "0x{:x}", uid.GetID());
	}
};
