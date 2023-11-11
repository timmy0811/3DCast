#pragma once

namespace Cast {
	class Timestep {
	public:
		Timestep(float time)
			: m_Time(time)
		{

		}

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMillis() const { return m_Time * 1000.f; }

		float AddSeconds(float sec) { m_Time += sec; return m_Time; }
		float AddMillis(float millis) { m_Time += millis * 1000.f; return m_Time * 1000; }

	private:
		float m_Time;
	};
}