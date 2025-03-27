#include "castpch.h"
#include "StartupBanner.h"

#include "3DCast/Platform/OpenGL/OpenGLContext.h"
#include "3DCast/Core/Log.h"

bool Cast::Core::StartupBanner::Init()
{
	int result = glfwInit();

	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	CAST_ASSERT(result, "Could not initialize GLFW.");

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	window = glfwCreateWindow(666, 442, "", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return false;
	}

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	int posX = (mode->width - windowWidth) / 2;
	int posY = (mode->height - windowHeight) / 2;

	glfwSetWindowPos(window, posX, posY);
	glfwMakeContextCurrent(window);

	context = new OpenGLContext(window);
	context->Init();

	screenGeometry.reset(API::Advanced::GBufferScreenGeometry::Create(800, 600));
	shader.reset(API::Core::Shader::Create("../3DCast/ressources/shader/sprite/basic_img.vert", "../3DCast/ressources/shader/sprite/basic_img.frag"));
	texture.reset(API::Texture::Texture::Create("../3DCast/ressources/img/startup.png", true));
	texture->Bind();
	shader->Bind();
	shader->SetUniform1i("uTexture", texture->GetBoundPort());

	return true;
}

void Cast::Core::StartupBanner::Blit(unsigned long ms)
{
	double startTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		if ((currentTime - startTime) * 1000.0 >= ms)
			break;

		glClear(GL_COLOR_BUFFER_BIT);

		screenGeometry->Draw(shader.get());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}