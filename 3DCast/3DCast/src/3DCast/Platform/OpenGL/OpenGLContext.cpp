#include "castpch.h"
#include "OpenGLContext.h"

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

Cast::OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	:m_WindowHandle(windowHandle)
{
	CAST_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void Cast::OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	glewExperimental = true;
	int res = glewInit();
	CAST_CORE_ASSERT(glewInit() == GLEW_OK, "Could not init glew.");
#ifndef CAST_ENABLE_ASSERTS
	if (res != GLEW_OK) {
		LOG_CORE_ERROR("Could not init glew.");
		return;
	}
#endif
	LOG_START_LOCAL_SW(sw);
	LOG_CORE_INFO("Detected OpenGL Version: {0}", std::string((const char*)glGetString(GL_VERSION)));
	LOG_CORE_INFO("Detected Renderer: {0}", std::string((const char*)glGetString(GL_RENDERER)));
	LOG_CORE_INFO("Detected GPU publisher: {0}", std::string((const char*)glGetString(GL_VENDOR)));
	LOG_CORE_INFO("Detected GLSL Version: {0}", std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	LOG_SW_ELAPSED(sw);
}

void Cast::OpenGLContext::SwapBuffer()
{
	glfwSwapBuffers(m_WindowHandle);
}