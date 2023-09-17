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
		m_Window->OnUpdate();
	}
}

void Cast::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

	LOG_CORE_TRACE("{0}", e.ToString());
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}