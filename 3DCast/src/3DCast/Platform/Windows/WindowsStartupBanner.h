#pragma once

#include "3DCast/Core/StartupBanner.h"

#include <GLFW/glfw3.h>

namespace Cast::Core {
	class WindowsStartupBanner : public StartupBanner
	{
	public:
		WindowsStartupBanner() = default;
		~WindowsStartupBanner() = default;

		bool Init(const std::string& imgPath) override;
		void Blit(unsigned long ms) override;

	private:
		GLFWwindow* window;
	};
}
