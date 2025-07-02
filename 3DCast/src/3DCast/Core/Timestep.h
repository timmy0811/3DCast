#pragma once

namespace Cast
{
	class Timestep
	{
	public:
		Timestep(const float time)
			: time(time)
		{
		}

		operator float() const { return time; }

		[[nodiscard]] float GetSeconds() const { return time; }
		[[nodiscard]] float GetMillis() const { return time * 1000.f; }

		float AddSeconds(const float sec)
		{
			time += sec;
			return time;
		}

		float AddMillis(const float millis)
		{
			time += millis * 1000.f;
			return time * 1000;
		}

	private:
		float time;
	};
}
