#pragma once

#include "3DCast/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Cast {
	class OpenGLContext final : public GraphicsContext {
	public:
		explicit OpenGLContext(GLFWwindow* windowHandle);

		void Init() override;
		void SwapBuffer() override;

	private:
		GLFWwindow* windowHandle;
	};
}