#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"
#include "Input.h"

#include "Renderer/Renderer.h"

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast {
	Application* Application::s_Instance = nullptr;
}

Cast::Application::Application(const WindowProperties& properties)
{
	CAST_CORE_ASSERT(!s_Instance, "Application is a singleton and cannot be instanced multiple times!");
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create(properties));
	m_Window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

void Cast::Application::Run()
{
	while (m_Running) {
		float time = (float)glfwGetTime();
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate(timestep);
		}

		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack) {
			layer->OnImGuiRender();
		}
		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}

	LOG_INFO("Terminating Application");
}

void Cast::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

	// LOG_CORE_TRACE("{0}", e.ToString());
	for (auto iter = m_LayerStack.end(); iter != m_LayerStack.begin();) {
		(*--iter)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Cast::Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Cast::Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

void Cast::Application::PopLayer(Layer* layer)
{
	m_LayerStack.PopLayer(layer);
	layer->OnDetach();
}

void Cast::Application::PopOverlay(Layer* overlay)
{
	m_LayerStack.PopOverlay(overlay);
	overlay->OnDetach();
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}