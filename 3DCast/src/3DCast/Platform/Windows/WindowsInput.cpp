#include "castpch.h"
#include "WindowsInput.h"

#include "3DCast/Application.h"
#include <GLFW/glfw3.h>

namespace Cast
{
#ifdef CAST_PLATFORM_WINDOWS
	Input* Input::instance = new WindowsInput();
#endif
}

bool Cast::WindowsInput::IsKeyPressedImpl(const int keyCode)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	const auto state = glfwGetKey(window, keyCode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Cast::WindowsInput::IsMouseButtonPressedImpl(const int keyCode)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	const auto state = glfwGetMouseButton(window, keyCode);
	return state == GLFW_PRESS;
}

float Cast::WindowsInput::GetMouseYImpl()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)yPos;
}

float Cast::WindowsInput::GetMouseXImpl()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)xPos;
}

std::pair<float, float> Cast::WindowsInput::GetMousePosImpl()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return std::pair((float)xPos, (float)yPos);
}
