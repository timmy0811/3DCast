#pragma once

#include "3DCast/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Cast {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffer() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}