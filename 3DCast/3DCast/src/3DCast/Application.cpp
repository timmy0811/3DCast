#include "castpch.h"
#include "Application.h"

#include "3DCast/Event/ApplicationEvent.h"
#include "3DCast/Log.h"

Cast::Application::Application()
{
}

Cast::Application::~Application()
{
}

void Cast::Application::Run()
{
	LOG_INFO("This is an event-test!");
	WindowResizeEvent e(1020, 900);
	LOG_INFO(e.ToString()); // Need to revisit

	while (true);
}