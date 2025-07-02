#include "castpch.h"
#include "LinuxInput.h"

#include "3DCast/Application.h"
#include <GLFW/glfw3.h>

namespace Cast
{
#ifdef CAST_PLATFORM_LINUX
	Input* Input::instance = new LinuxInput();
#endif
}

bool Cast::LinuxInput::IsKeyPressedImpl(const int keyCode)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	const auto state = glfwGetKey(window, keyCode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Cast::LinuxInput::IsMouseButtonPressedImpl(const int keyCode)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	const auto state = glfwGetMouseButton(window, keyCode);
	return state == GLFW_PRESS;
}

float Cast::LinuxInput::GetMouseYImpl()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)yPos;
}

float Cast::LinuxInput::GetMouseXImpl()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)xPos;
}

std::pair<float, float> Cast::LinuxInput::GetMousePosImpl()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return std::pair((float)xPos, (float)yPos);
}
