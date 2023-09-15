#include "castpch.h"
#include "WindowsWindow.h"

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
		CAST_ASSERT(result, "Could not initialize GLFW!");

		s_GLFWInitialized = true;
	}

	m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(m_Window);
	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);
}

void Cast::WindowsWindow::Destroy()
{
	glfwDestroyWindow(m_Window);
}

Cast::Window* Cast::Window::Create(const WindowProperties& props) {
	return new WindowsWindow(props);
}