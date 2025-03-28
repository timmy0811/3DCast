#pragma once

#include <API/advanced/GBufferScreenGeometry.h>
#include <API/texture/Texture.h>

#include "3DCast/Renderer/GraphicsContext.h"

namespace Cast::Core {
	class StartupBanner
	{
	public:
		virtual ~StartupBanner() {};

		virtual bool Init(const std::string& imgPath) = 0;
		virtual void Blit(unsigned long ms) = 0;

		static Scope<StartupBanner> Create();

	protected:
		GraphicsContext* context;

		Ref<API::Advanced::GBufferScreenGeometry> screenGeometry;
		Ref<API::Core::Shader> shader;
		Ref<API::Texture::Texture> texture;
	};
}