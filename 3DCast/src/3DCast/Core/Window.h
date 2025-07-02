#pragma once

#include "3DCast/Core.h"
#include "3DCast/Event/Event.h"

#include "vendor/glm/glm.hpp"

namespace Cast
{
	struct WindowProperties
	{
		std::string Title;
		unsigned int Width, Height;

		explicit WindowProperties(const std::string& title = "Window", const unsigned int width = 1280,
		                          const unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;
		[[nodiscard]] virtual unsigned int GetWidth() const = 0;
		[[nodiscard]] virtual unsigned int GetHeight() const = 0;

		[[nodiscard]] virtual glm::ivec2 GetPosition() const = 0;

		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		[[nodiscard]] virtual bool IsVSync() const = 0;

		virtual void SetRawMouseInput(bool enabled) const = 0;
		virtual void SetInputModeDisabled() const = 0;
		virtual void SetInputModeNormal() const = 0;

		virtual void SetCursorPosition(double xpos, double ypos) = 0;

		[[nodiscard]] virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProperties& props = WindowProperties());
	};
}
