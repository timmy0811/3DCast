#pragma once

#include "3DCast/Input.h"

namespace Cast {
	class WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(int keyCode) override;
		virtual bool IsMouseButtonPressedImpl(int keyCode) override;
		virtual float GetMouseYImpl() override;
		virtual float GetMouseXImpl() override;

		// Inherited via Input
		std::pair<float, float> GetMousePosImpl() override;
	};
}
