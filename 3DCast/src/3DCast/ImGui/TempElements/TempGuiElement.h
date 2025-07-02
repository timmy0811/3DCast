#pragma once

namespace Cast::GUI {
	class TempGuiElement {
	public:
		virtual ~TempGuiElement() = default;
		TempGuiElement() = default;

		// returns true if element is closed
		virtual bool Render() = 0;
	};
}