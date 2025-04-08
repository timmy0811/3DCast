#pragma once

namespace Cast::GUI {
	class TempGuiElement {
	public:
		TempGuiElement() = default;

		// returns true if element is closed
		virtual bool Render() = 0;
	};
}