#include "castpch.h"

#include "WindowsWindow.h"

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
	glfwSwapBuffers(m_Window);
}

void Cast::WindowsWindow::SetVSync(bool enabled)
{
	if (enabled) glfwSwapInterval(1);
	else glfwSwapInterval(0);

	m_Data.VSync = enabled;
}

inline bool Cast::WindowsWindow::IsVSync() const
{
	return m_Data.VSync;
}

void Cast::WindowsWindow::Init(const WindowProperties& props)
{
	m_Data.Title = props.Title;
	m_Data.Width = props.Width;
	m_Data.Height = props.Height;

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

	m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
	if (!m_Window) {
		glfwTerminate();
		CAST_ASSERT(false, "Could not initialize Window.");
	}

	glfwMakeContextCurrent(m_Window);
	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Could not init glew." << std::endl;
		__debugbreak;
	}

	LOG_CORE_INFO("Detected OpenGL Version: {0}", *glGetString(GL_VERSION));

	// GLFW Callbacks
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height);
		data.EventCallback(event);
		});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.EventCallback(event);
		});

	glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int c) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		KeyTypedEvent event(c);
		data.EventCallback(event);
		});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
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

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
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

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.EventCallback(event);
		});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event((float)xPos, (float)yPos);
		data.EventCallback(event);
		});
}

void Cast::WindowsWindow::Destroy()
{
	glfwDestroyWindow(m_Window);
}

void* Cast::WindowsWindow::GetNativeWindow() const
{
	return m_Window;
}

Cast::Window* Cast::Window::Create(const WindowProperties& props) {
	return new WindowsWindow(props);
}