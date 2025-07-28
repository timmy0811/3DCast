#include "castpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "3DCast/Application.h"

// Temp
#include <GLFW/glfw3.h>

#include <ImGuizmo.h>

Cast::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer")
{
}

Cast::ImGuiLayer::~ImGuiLayer() = default;

void Cast::ImGuiLayer::OnAttach()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigWindowsMoveFromTitleBarOnly = true;
#ifdef CAST_PLATFORM_WINDOWS
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
#endif
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	const ImFont* myFont = io.Fonts->AddFontFromFileTTF((std::string(ASSET_DIR) + "font/roboto/ubuntu.ttf").c_str(),
	                                                    15.5f);
	if (myFont == nullptr)
		myFont = io.Fonts->AddFontDefault();

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	const Application& app = Application::Get();
	auto* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
	ImGui_ImplOpenGL3_CreateDeviceObjects();

	ImGuizmo::SetOrthographic(false);
}

void Cast::ImGuiLayer::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Cast::ImGuiLayer::OnImGuiRender()
{
}

void Cast::ImGuiLayer::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();
}

void Cast::ImGuiLayer::End()
{
	ImGuiIO& io = ImGui::GetIO();
	const Application& app = Application::Get();
	const int currentWidth = (int)app.GetWindow().GetWidth();
	const int currentHeight = (int)app.GetWindow().GetHeight();

	if (LastWindowWidth != currentWidth || LastWindowHeight != currentHeight) {
		io.DisplaySize = ImVec2((float)currentWidth, (float)currentHeight);
		LastWindowWidth = currentWidth;
		LastWindowHeight = currentHeight;
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
