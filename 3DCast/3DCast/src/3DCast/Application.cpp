#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

Cast::Application::Application()
{
	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));
}

Cast::Application::~Application()
{
}

void Cast::Application::Run()
{
	while (m_Running) {
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
}

void Cast::Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
}

void Cast::Application::PopLayer(Layer* layer)
{
	m_LayerStack.PopLayer(layer);
}

void Cast::Application::PopOverlay(Layer* overlay)
{
	m_LayerStack.PopOverlay(overlay);
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}