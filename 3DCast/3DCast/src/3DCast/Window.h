#pragma once

#include "castpch.h"

#include "3DCast/Core.h"
#include "3DCast/Event/Event.h"

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

		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProperties& props = WindowProperties());
	};
}