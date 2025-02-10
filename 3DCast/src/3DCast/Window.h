#pragma once

#include "3DCast/Core.h"
#include "3DCast/Event/Event.h"

#include "Vendor/glm/glm.hpp"

namespace Cast {
	struct WindowProperties {
		std::string Title;
		unsigned int Width, Height;

		WindowProperties(const std::string& title = "Window", unsigned int width = 1280, unsigned int height = 720)
			:Title(title), Width(width), Height(height) {}
	};

	class Window {
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual glm::ivec2 GetPosition() const = 0;

		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetRawMouseInput(bool enabled) const = 0;
		virtual void SetInputModeDisabled() const = 0;
		virtual void SetInputModeNormal() const = 0;

		virtual void SetCursorPosition(double xpos, double ypos) = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProperties& props = WindowProperties());
	};
}