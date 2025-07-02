#pragma once

#include "3DCast/Core/StartupBanner.h"

#include <GLFW/glfw3.h>

namespace Cast::Core {
	class LinuxStartupBanner final : public StartupBanner
	{
	public:
		LinuxStartupBanner() = default;
		~LinuxStartupBanner() override = default;

		bool Init(const std::string& imgPath) override;
		void Blit(unsigned long ms) override;

	private:
		GLFWwindow* window = nullptr;
	};
}
