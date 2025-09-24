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

inline glm::ivec2 Cast::WindowsWindow::GetPosition() const
{
	glm::ivec2 pos;
	glfwGetWindowPos(window, &pos.x, &pos.y);
	return pos;
}

void Cast::WindowsWindow::SetVSync(const bool enabled)
{
	glfwSwapInterval(enabled ? 1 : 0);
	data.VSync = enabled;
}

inline bool Cast::WindowsWindow::IsVSync() const
{
	return data.VSync;
}

inline void Cast::WindowsWindow::SetRawMouseInput(const bool enabled) const
{
	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
	}
}

void Cast::WindowsWindow::SetInputModeDisabled() const
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Cast::WindowsWindow::SetInputModeNormal() const
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Cast::WindowsWindow::SetTitle(const char* title) const
{
	glfwSetWindowTitle(window, title);
}

void Cast::WindowsWindow::ResetToDefaultTitle() const
{
	glfwSetWindowTitle(window, "3DCast Rendering Engine");
}

void Cast::WindowsWindow::SetCursorPosition(const double xpos, const double ypos)
{
	glfwSetCursorPos(window, xpos, ypos);
}

void Cast::WindowsWindow::Init(const WindowProperties& props)
{
	data.Title = props.Title;
	data.Width = props.Width;
	data.Height = props.Height;

	LOG_CORE_INFO("Initializing Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

	if (!s_GLFWInitialized)
	{
		const int result = glfwInit();

		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		CAST_ASSERT(result, "Could not initialize GLFW.");

		glfwSetErrorCallback(GLFWErrorCallback);

		s_GLFWInitialized = true;
	}

	window = glfwCreateWindow((int)props.Width, (int)props.Height, data.Title.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		CAST_ASSERT(false, "Could not initialize Window.");
	}

	context = new OpenGLContext(window);
	context->Init();

	glfwSetWindowUserPointer(window, &data);
	SetVSync(false);

	// GLFW Callbacks
	glfwSetWindowPosCallback(window, [](GLFWwindow* window, const int xPos, const int yPos)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		WindowMovedEvent event(xPos, yPos);
		data.EventCallback(event);
	});

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, const int width, const int height)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height);
		data.EventCallback(event);
	});

	glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.EventCallback(event);
	});

	glfwSetCharCallback(window, [](GLFWwindow* window, const unsigned int c)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		KeyTypedEvent event((int)c);
		data.EventCallback(event);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, const int key, int scancode, const int action, int mods)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
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
		default: ;
		}
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, const int button, const int action, int mods)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
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
		default: ;
		}
	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, const double xOffset, const double yOffset)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.EventCallback(event);
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, const double xPos, const double yPos)
	{
		const WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event((float)xPos, (float)yPos);
		data.EventCallback(event);
	});
}

void Cast::WindowsWindow::Destroy() const
{
	glfwDestroyWindow(window);
}

void* Cast::WindowsWindow::GetNativeWindow() const
{
	return window;
}
