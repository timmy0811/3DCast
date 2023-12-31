#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "3DCast/Window.h"
#include "3DCast/Renderer/GraphicsContext.h"

namespace Cast {
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* desc) {
		LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, desc);
	}

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

		inline virtual void* GetNativeWindow() const override;

	private:
		GLFWwindow* m_Window;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunc EventCallback;
		};

		WindowData m_Data;
		GraphicsContext* m_Context;
	};
}
