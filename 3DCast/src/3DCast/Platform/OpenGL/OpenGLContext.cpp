#include "castpch.h"
#include "OpenGLContext.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Cast::OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: windowHandle(windowHandle)
{
	CAST_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void Cast::OpenGLContext::Init()
{
	glfwMakeContextCurrent(windowHandle);
	if (!windowHandle)
	{
		LOG_CORE_ERROR("Failed to init GLFW Context.");
	}

#ifdef CAST_ENABLE_ASSERTS
	CAST_CORE_ASSERT(windowHandle, "Could not init GLFW Context.");
#endif

	glewExperimental = true;
	if (const unsigned int res = glewInit(); res != GLEW_OK)
	{
		LOG_CORE_ERROR("Could not init glew: " + (std::stringstream() << glewGetErrorString(res)).str());
		return;
	}

#ifdef CAST_ENABLE_ASSERTS
	CAST_CORE_ASSERT(glewInit() == GLEW_OK, "Could not init glew.");
#endif

	LOG_START_LOCAL_SW(sw);
	LOG_CORE_INFO("Detected OpenGL Version: {0}", std::string((const char*)glGetString(GL_VERSION)));
	LOG_CORE_INFO("Detected Renderer: {0}", std::string((const char*)glGetString(GL_RENDERER)));
	LOG_CORE_INFO("Detected GPU publisher: {0}", std::string((const char*)glGetString(GL_VENDOR)));
	LOG_CORE_INFO("Detected GLSL Version: {0}", std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	LOG_SW_ELAPSED(sw);

	if (glewIsSupported("GL_ARB_bindless_texture"))
	{
		LOG_CORE_INFO("Your GPU kernel supports bindless textures. Great");
	}
	else
	{
		LOG_CORE_WARN(
			"Your GPU kernel does not support bindless textures. This may cause the application to not work properly.");
	}

	glEnable(GL_MULTISAMPLE);
}

void Cast::OpenGLContext::SwapBuffer()
{
	glfwSwapBuffers(windowHandle);
}
