#pragma once

#include "3DCast/Window.h"

#include <GLFW/glfw3.h>

namespace Cast {
	static bool s_GLFWInitialized = false;

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& props);
		virtual ~WindowsWindow() {}

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; };
		inline unsigned int GetHeight() const override { return m_Data.Height; };

		inline void SetEventCallback(const EventCallbackFunc& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		inline bool IsVSync() const override;

	private:
		virtual void Init(const WindowProperties& props);
		virtual void Destroy();

	private:
		GLFWwindow* m_Window;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunc EventCallback;
		};

		WindowData m_Data;
	};
}
