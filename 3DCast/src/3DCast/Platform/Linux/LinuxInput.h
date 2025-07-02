#pragma once

#include "3DCast/Input/Input.h"

namespace Cast
{
	class LinuxInput final : public Input
	{
	protected:
		bool IsKeyPressedImpl(int keyCode) override;
		bool IsMouseButtonPressedImpl(int keyCode) override;
		float GetMouseYImpl() override;
		float GetMouseXImpl() override;

		// Inherited via Input
		std::pair<float, float> GetMousePosImpl() override;
	};
}
