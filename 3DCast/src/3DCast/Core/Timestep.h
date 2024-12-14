#pragma once

namespace Cast {
	class Timestep {
	public:
		Timestep(float time)
			: time(time)
		{

		}

		operator float() const { return time; }

		float GetSeconds() const { return time; }
		float GetMillis() const { return time * 1000.f; }

		float AddSeconds(float sec) { time += sec; return time; }
		float AddMillis(float millis) { time += millis * 1000.f; return time * 1000; }

	private:
		float time;
	};
}