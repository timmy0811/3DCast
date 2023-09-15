#pragma once

#include "Core.h"
#include "Event/Event.h"
#include "Window.h"

namespace Cast {
	class CAST_API Application
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	Application* CreatApplication();
}
