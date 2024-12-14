#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"
#include "Input.h"

#include "Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast {
	Application* Application::instance = nullptr;
}

Cast::Application::Application(const WindowProperties& properties)
{
	CAST_CORE_ASSERT(!instance, "Application is a singleton and cannot be instanced multiple times!");
	instance = this;

	window = std::unique_ptr<Window>(Window::Create(properties));
	window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

	Renderer::RendererContext::Init();

	imGuiLayer = new ImGuiLayer();
	PushOverlay(imGuiLayer);
}

void Cast::Application::Run()
{
	while (running) {
		float time = (float)glfwGetTime();
		Timestep timestep = time - lastFrameTime;
		lastFrameTime = time;

		for (Layer* layer : layerStack) {
			layer->OnUpdate(timestep);
		}

		imGuiLayer->Begin();
		for (Layer* layer : layerStack) {
			layer->OnImGuiRender();
		}
		imGuiLayer->End();

		window->OnUpdate();
	}

	LOG_INFO("Terminating Application");
}

void Cast::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

	// LOG_CORE_TRACE("{0}", e.ToString());
	for (auto iter = layerStack.end(); iter != layerStack.begin();) {
		(*--iter)->OnEvent(e);
		if (e.handled)
			break;
	}
}

void Cast::Application::PushLayer(Layer* layer)
{
	layerStack.PushLayer(layer);
	layer->OnAttach();
}

void Cast::Application::PushOverlay(Layer* overlay)
{
	layerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

void Cast::Application::PopLayer(Layer* layer)
{
	layerStack.PopLayer(layer);
	layer->OnDetach();
}

void Cast::Application::PopOverlay(Layer* overlay)
{
	layerStack.PopOverlay(overlay);
	overlay->OnDetach();
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
	running = false;
	return true;
}