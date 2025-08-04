#pragma once
#include <string>
#include <vector>

namespace Runtime::GUI {
	class EventConsole {
	public:
		EventConsole() = delete;

		static void OnImGuiRender();
	};
}
