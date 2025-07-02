#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "3DCast/Core/Window.h"
#include "3DCast/Renderer/GraphicsContext.h"

namespace Cast
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* desc)
	{
		LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, desc);
	}

	class LinuxWindow final : public Window
	{
	public:
		explicit LinuxWindow(const WindowProperties& props);

		~LinuxWindow() override = default;

		void OnUpdate() override;

		[[nodiscard]] inline unsigned int GetWidth() const override { return data.Width; };
		[[nodiscard]] inline unsigned int GetHeight() const override { return data.Height; };

		[[nodiscard]] inline glm::ivec2 GetPosition() const override;

		inline void SetEventCallback(const EventCallbackFunc& callback) override { data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		[[nodiscard]] inline bool IsVSync() const override;

		inline void SetRawMouseInput(bool enabled) const override;
		void SetInputModeDisabled() const override;
		void SetInputModeNormal() const override;

		void SetCursorPosition(double xpos, double ypos) override;

	private:
		void Init(const WindowProperties& props);
		void Destroy() const;

		[[nodiscard]] inline void* GetNativeWindow() const override;

	private:
		GLFWwindow* window{};

		struct WindowData
		{
			std::string Title = "";
			unsigned int Width = 0, Height = 0;
			bool VSync = false;

			EventCallbackFunc EventCallback;
		};

		WindowData data;
		GraphicsContext* context{};
	};
}
