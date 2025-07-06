#include "castpch.h"
#include "WindowsStartupBanner.h"

#include "3DCast/Platform/OpenGL/OpenGLContext.h"
#include "3DCast/Core/Log.h"

bool Cast::Core::WindowsStartupBanner::Init(const std::string& imgPath)
{
	const int result = glfwInit();

	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	CAST_ASSERT(result, "Could not initialize GLFW.");

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	window = glfwCreateWindow(666, 442, "", nullptr, nullptr);
	if (!window)
	{
		LOG_CORE_ERROR("Could not initialize Window.");
		glfwTerminate();
		return false;
	}

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	const int posX = (mode->width - windowWidth) / 2;
	const int posY = (mode->height - windowHeight) / 2;

	glfwSetWindowPos(window, posX, posY);
	glfwMakeContextCurrent(window);

	context = new OpenGLContext(window);
	context->Init();

	screenGeometry.reset(API::Advanced::GBufferScreenGeometry::Create(880, 660));
	shader.reset(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/sprite/basic_img.vert", std::string(ASSET_DIR) + "shader/sprite/basic_img.frag"));
	texture.reset(API::Texture::Texture::Create(imgPath, API::Texture::TextureFilter::LINEAR, true));
	texture->Bind();
	shader->Bind();
	shader->SetUniform1i("uTexture", texture->GetBoundPort());

	return true;
}

void Cast::Core::WindowsStartupBanner::Blit(const unsigned long ms)
{
	const double startTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		if ((glfwGetTime() - startTime) * 1000.0 >= ms)
			break;

		glClear(GL_COLOR_BUFFER_BIT);

		screenGeometry->Draw(shader.get());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}