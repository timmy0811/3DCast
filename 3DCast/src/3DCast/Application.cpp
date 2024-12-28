#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"
#include "Input.h"

#include "Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast {
	Application* Application::Instance = nullptr;
}

Cast::Application::Application(const WindowProperties& properties)
{
	CAST_CORE_ASSERT(!Instance, "Application is a singleton and cannot be instanced multiple times!");
	Instance = this;

	AppWindow = std::unique_ptr<Window>(Window::Create(properties));
	AppWindow->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

	Renderer::RendererContext::Init();

	GuiLayer = new ImGuiLayer();
	PushOverlay(GuiLayer);
}

void Cast::Application::Run()
{
	while (Running) {
		float time = (float)glfwGetTime();
		Timestep timestep = time - LastFrameTime;
		LastFrameTime = time;

		if (!Minimized) {
			for (Layer* layer : LayerStack) {
				layer->OnUpdate(timestep);
			}
		}

		GuiLayer->Begin();
		for (Layer* layer : LayerStack) {
			layer->OnImGuiRender();
		}
		GuiLayer->End();

		AppWindow->OnUpdate();
	}

	LOG_INFO("Terminating Application");
}

void Cast::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OnWindowResize));

	for (auto iter = LayerStack.end(); iter != LayerStack.begin();) {
		(*--iter)->OnEvent(e);
		if (e.handled)
			break;
	}
}

void Cast::Application::Close()
{
	Running = false;
}

void Cast::Application::PushLayer(Layer* layer)
{
	layer->SetParentWindow(AppWindow);
	LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Cast::Application::PushOverlay(Layer* overlay)
{
	LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

void Cast::Application::PopLayer(Layer* layer)
{
	LayerStack.PopLayer(layer);
	layer->OnDetach();
}

void Cast::Application::PopOverlay(Layer* overlay)
{
	LayerStack.PopOverlay(overlay);
	overlay->OnDetach();
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
	Running = false;
	return true;
}

bool Cast::Application::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0) {
		Minimized = true;
		return false;
	}

	Minimized = false;
	Renderer::RendererContext::OnWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}