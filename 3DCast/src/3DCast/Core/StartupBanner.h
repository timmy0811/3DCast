#pragma once
#include "GLFW/glfw3.h"
#include "3DCast/Renderer/GraphicsContext.h"
#include "API/advanced/GBufferScreenGeometry.h"
#include "API/texture/Texture.h"

namespace Cast::Core {
	class StartupBanner
	{
	public:
		StartupBanner() = default;
		~StartupBanner() = default;

		bool Init();
		void Blit(unsigned long ms);

	private:
		GLFWwindow* window;
		GraphicsContext* context;

		Ref<API::Advanced::GBufferScreenGeometry> screenGeometry;
		Ref<API::Core::Shader> shader;
		Ref<API::Texture::Texture> texture;
	};
}