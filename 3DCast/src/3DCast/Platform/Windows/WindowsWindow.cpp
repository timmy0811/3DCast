#include "castpch.h"

#include "WindowsWindow.h"
#include "3DCast/Platform/OpenGL/OpenGLContext.h"

#include "3DCast/Event/ApplicationEvent.h"
#include "3DCast/Event/MouseEvent.h"
#include "3DCast/Event/KeyEvent.h"

Cast::WindowsWindow::WindowsWindow(const WindowProperties& props)
{
	Init(props);
}

void Cast::WindowsWindow::OnUpdate()
{
	glfwPollEvents();
	context->SwapBuffer();
}

void Cast::WindowsWindow::SetVSync(bool enabled)
{
	if (enabled) glfwSwapInterval(1);
	else glfwSwapInterval(0);

	data.VSync = enabled;
}

inline bool Cast::WindowsWindow::IsVSync() const
{
	return data.VSync;
}

void Cast::WindowsWindow::SetInputModeDisabled() const
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Cast::WindowsWindow::SetInputModeNormal() const
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Cast::WindowsWindow::Init(const WindowProperties& props)
{
	data.Title = props.Title;
	data.Width = props.Width;
	data.Height = props.Height;

	LOG_CORE_INFO("Initializing Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

	if (!s_GLFWInitialized) {
		int result = glfwInit();

		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		CAST_ASSERT(result, "Could not initialize GLFW.");

		glfwSetErrorCallback(GLFWErrorCallback);

		s_GLFWInitialized = true;
	}

	window = glfwCreateWindow((int)props.Width, (int)props.Height, data.Title.c_str(), nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		CAST_ASSERT(false, "Could not initialize Window.");
	}

	context = new OpenGLContext(window);
	context->Init();

	glfwSetWindowUserPointer(window, &data);
	SetVSync(true);

	// GLFW Callbacks
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height);
		data.EventCallback(event);
		});

	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.EventCallback(event);
		});

	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int c) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		KeyTypedEvent event(c);
		data.EventCallback(event);
		});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action) {
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			data.EventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, 1);
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			data.EventCallback(event);
			break;
		}
		}
		});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action) {
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(button);
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(button);
			data.EventCallback(event);
			break;
		}
		}
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.EventCallback(event);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event((float)xPos, (float)yPos);
		data.EventCallback(event);
		});
}

void Cast::WindowsWindow::Destroy()
{
	glfwDestroyWindow(window);
}

void* Cast::WindowsWindow::GetNativeWindow() const
{
	return window;
}