#pragma once

namespace Cast
{
	typedef uint32_t uid;

	class UID
	{
	public:
		UID() = delete;
		~UID() = delete;

		inline static uid Create() { return GlobalId++; }
		inline static uid None() { return 0; }

	private:
		static inline uint32_t GlobalId = 1;
	};
}
