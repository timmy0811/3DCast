#pragma once

#include "3DCast/Core.h"

namespace Cast {
	class Input {
	public:
		inline static bool IsKeyPressed(int keyCode) { return instance->IsKeyPressedImpl(keyCode); }
		inline static bool IsMouseButtonPressed(int keyCode) { return instance->IsMouseButtonPressedImpl(keyCode); }
		inline static float GetMouseX() { return instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return instance->GetMouseYImpl(); }
		inline static std::pair<float, float> GetMousePos() { return instance->GetMousePosImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keyCode) = 0;
		virtual bool IsMouseButtonPressedImpl(int keyCode) = 0;
		virtual float GetMouseYImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;

	private:
		static Input* instance;
	};
}