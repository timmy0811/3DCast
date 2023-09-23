#include "castpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "GLFW/glfw3.h"
#include "3DCast/Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include "3DCast/Application.h"

// Temp
#include <GLFW/glfw3.h>

Cast::ImGuiLayer::ImGuiLayer()
	:Layer("ImGuiLayer")
{
}

Cast::ImGuiLayer::~ImGuiLayer()
{
}

void Cast::ImGuiLayer::OnUpdate()
{
	ImGuiIO& io = ImGui::GetIO();
	Application& application = Application::Get();
	io.DisplaySize = ImVec2(application.GetWindow().GetWidth(), application.GetWindow().GetHeight());

	float time = (float)glfwGetTime();
	io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.f / 60.f);
	m_Time = time;

	ImGui_ImplOpenGL3_NewFrame();

	ImGui::NewFrame();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Cast::ImGuiLayer::OnEvent(Event& e)
{
	EventDispatcher dp(e);
	dp.Dispatch<MouseButtonPressedEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressedEvent));
	dp.Dispatch<MouseButtonReleasedEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleasedEvent));
	dp.Dispatch<MouseMovedEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnMouseMovedEvent));
	dp.Dispatch<MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolledEvent));
	dp.Dispatch<KeyPressedEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressedEvent));
	dp.Dispatch<KeyReleasedEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleasedEvent));
	dp.Dispatch<WindowResizeEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnWindowResizeEvent));
	dp.Dispatch<KeyTypedEvent>(CAST_BIND_EVENT_FUNC(ImGuiLayer::OnKeyTypedEvent));
}

void Cast::ImGuiLayer::OnAttach()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	ImGui_ImplOpenGL3_Init("#version 410");
}

void Cast::ImGuiLayer::OnDetach()
{
}

bool Cast::ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = true;
	return false;
}

bool Cast::ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = false;
	return false;
}

bool Cast::ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(e.GetX(), e.GetY());
	return false;
}

bool Cast::ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += e.GetXOffset();
	io.MouseWheelH += e.GetYOffset();
	return false;
}

bool Cast::ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.GetKeyCode()] = true;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	return false;
}

bool Cast::ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();

	int keyCode = e.GetKeyCode();
	if (keyCode > 0 && keyCode < 0x10000) {
		io.AddInputCharacter((unsigned short)keyCode);
	}
	return false;
}

bool Cast::ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.GetKeyCode()] = false;
	return false;
}

bool Cast::ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
	io.DisplayFramebufferScale = ImVec2(1.f, 1.f);
	glViewport(0, 0, e.GetWidth(), e.GetHeight());
	return false;
}