#pragma once

#include "Core.h"
#include "Event/Event.h"
#include "Window.h"
#include "3DCast/Event/ApplicationEvent.h"

namespace Cast {
	class CAST_API Application
	{
	public:
		Application();
		~Application();

		void Run();
		void OnEvent(Event& e);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	Application* CreatApplication();
}
