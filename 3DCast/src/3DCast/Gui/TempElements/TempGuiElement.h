#pragma once

namespace Cast::GUI {
	class TempGuiElement {
	public:
		virtual ~TempGuiElement() = default;
		TempGuiElement() = default;

		virtual void SetTextAttrib(const char* text) = 0;

		// returns true if element is closed
		virtual bool Render() = 0;
	};
}