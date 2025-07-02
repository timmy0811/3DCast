#pragma once

#include "3DCast/Core.h"

#include <API/advanced/GBuffer.h>
#include <API/advanced/GBufferScreenGeometry.h>
#include <API/core/Framebuffer.h>

namespace Cast::Renderer {
	struct RasterizationPipelineObjects
	{
		Ref<API::Core::Framebuffer> Framebuffer;
		Ref<API::Advanced::GBuffer> GBuffer;
		Ref<API::Advanced::GBufferScreenGeometry> GBufferScreenGeometry;
	};
}