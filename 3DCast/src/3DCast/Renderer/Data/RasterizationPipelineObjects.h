#pragma once

#include "3DCast/Core.h"

#include <API/advanced/GBuffer.h>
#include <API/advanced/GBufferScreenGeometry.h>
#include <API/core/Framebuffer.h>

namespace Cast::Renderer {
	struct RasterizationPipelineObjects
	{
		Cast::Ref<API::Core::Framebuffer> Framebuffer;
		Cast::Ref<API::Advanced::GBuffer> GBuffer;
		Cast::Ref<API::Advanced::GBufferScreenGeometry> GBufferScreenGeometry;
	};
}