#pragma once

#include "Core/Log.h"
#include <memory>
#include <optional>

#ifdef CAST_ENABLE_ASSERTS
#ifdef CAST_PLATFORM_WINDOWS
#define CAST_ASSERT(x, ...) {if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CAST_CORE_ASSERT(x, ...) {if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#elif CAST_PLATFORM_LINUX
#include <csignal>
#define CAST_ASSERT(x, ...) {if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP); } }
#define CAST_CORE_ASSERT(x, ...) {if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP); } }

#endif

#else
#define CAST_ASSERT(x, ...) { }
#define CAST_CORE_ASSERT(x, ...) { }
#endif

#define BIT(x) (1 << x)

#define CAST_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Cast
{
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using WeakRef = std::weak_ptr<T>;

	template <typename T>
	class Optional
	{
	public:
		// Constructors
		Optional() = default;
		Optional(const Optional&) = default;
		Optional(Optional&&) noexcept = default;
		Optional(const std::optional<T>& other) : Option(other) {}
		Optional(std::optional<T>&& other) noexcept : Option(std::move(other)) {}
		Optional(const T& value) : Option(value) {}
		Optional(T&& value) noexcept : Option(std::move(value)) {}

		// Assignment operators
		Optional& operator=(const Optional&) = default;
		Optional& operator=(Optional&&) noexcept = default;
		Optional& operator=(const T& value) { Option = value; return *this; }
		Optional& operator=(T&& value) noexcept { Option = std::move(value); return *this; }

		// Observers
		constexpr const T* operator->() const { return Option.operator->(); }
		constexpr T* operator->() { return Option.operator->(); }
		constexpr const T& operator*() const & { return *Option; }
		constexpr T& operator*() & { return *Option; }
		constexpr const T&& operator*() const && { return *Option; }
		constexpr T&& operator*() && { return *Option; }

		constexpr operator bool() const noexcept { return Option.has_value(); }
		[[nodiscard]] constexpr bool has_value() const noexcept { return Option.has_value(); }

		constexpr T& value() & { return Option.value(); }
		constexpr const T& value() const & { return Option.value(); }
		constexpr T&& value() && { return std::move(Option).value(); }
		constexpr const T&& value() const && { return std::move(Option).value(); }

		template<class U>
		constexpr T value_or(U&& default_value) const & { return Option.value_or(std::forward<U>(default_value)); }

		template<class U>
		constexpr T value_or(U&& default_value) && { return std::move(Option).value_or(std::forward<U>(default_value)); }

		// Modifiers
		constexpr void reset() noexcept { Option.reset(); }

		template<class... Args>
		constexpr T& emplace(Args&&... args) { return Option.emplace(std::forward<Args>(args)...); }

		// Get underlying std::optional
		constexpr const std::optional<T>& native() const & noexcept { return Option; }
		constexpr std::optional<T>& native() & noexcept { return Option; }

	private:
		std::optional<T> Option;
	};

	template<class T>
	constexpr Optional<std::decay_t<T>> MakeOptional(T&& value)
	{
		return Optional<std::decay_t<T>>(std::forward<T>(value));
	}

	template<class T, class... Args>
	constexpr Optional<T> MakeOptional(Args&&... args)
	{
		return Optional<T>(T(std::forward<Args>(args)...));
	}
}
