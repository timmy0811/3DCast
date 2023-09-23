#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"

#include <GLEW/glew.h>

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast {
	Application* Application::s_Instance = nullptr;
}

Cast::Application::Application()
{
	CAST_CORE_ASSERT(!s_Instance, "Application is a singleton and cannot be instanced multiple times!");
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));
}

Cast::Application::~Application()
{
}

void Cast::Application::Run()
{
	while (m_Running) {
		glClearColor(1.0, 0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
		}

		m_Window->OnUpdate();
	}
}

void Cast::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

	LOG_CORE_TRACE("{0}", e.ToString());
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