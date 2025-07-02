#pragma once

namespace Runtime::GUI {
	class EventConsole {
	public:
		EventConsole() = delete;

		static void OnImGuiRender();
	};
}
