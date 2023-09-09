#pragma once

#include "Core.h"

namespace Cast {
	class CAST_API Application
	{
	public:
		Application();
		~Application();

		void Run();
	};

	Application* CreatApplication();
}
