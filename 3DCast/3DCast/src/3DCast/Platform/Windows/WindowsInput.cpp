#include "castpch.h"
#include "WindowsInput.h"

#include "3DCast/Application.h"
#include <GLFW/glfw3.h>

namespace Cast {
	Input* Input::s_Instance = new WindowsInput();
}

bool Cast::WindowsInput::IsKeyPressedImpl(int keyCode)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, keyCode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Cast::WindowsInput::IsMouseButtonPressedImpl(int keyCode)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(window, keyCode);
	return state == GLFW_PRESS;
}

float Cast::WindowsInput::GetMouseYImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)yPos;
}

float Cast::WindowsInput::GetMouseXImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)xPos;
}

std::pair<float, float> Cast::WindowsInput::GetMousePosImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return std::pair<float, float>((float)xPos, (float)yPos);
}