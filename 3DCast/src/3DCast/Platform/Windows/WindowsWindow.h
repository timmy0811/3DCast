#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "3DCast/Core/Window.h"
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

		inline unsigned int GetWidth() const override { return data.Width; };
		inline unsigned int GetHeight() const override { return data.Height; };

		inline glm::ivec2 GetPosition() const override;

		inline void SetEventCallback(const EventCallbackFunc& callback) override { data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		inline bool IsVSync() const override;

		inline void SetRawMouseInput(bool enabled) const override;
		virtual void SetInputModeDisabled() const override;
		virtual void SetInputModeNormal() const override;

		virtual void SetCursorPosition(double xpos, double ypos) override;

	private:
		virtual void Init(const WindowProperties& props);
		virtual void Destroy();

		inline virtual void* GetNativeWindow() const override;

	private:
		GLFWwindow* window;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunc EventCallback;
		};

		WindowData data;
		GraphicsContext* context;
	};
}
