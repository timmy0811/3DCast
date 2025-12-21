#pragma once

#include "3DCast/Core.h"

#include <API/advanced/GBuffer.h>
#include <API/advanced/GBufferScreenGeometry.h>
#include <API/core/Framebuffer.h>
#include <API/advanced/SSAO.h>

namespace Cast::Renderer {
	struct RasterizationPipelineObjects
	{
		Ref<API::Core::Framebuffer> Framebuffer;
		Ref<API::Advanced::GBuffer> GBuffer;
		Ref<API::Advanced::GBufferScreenGeometry> GBufferScreenGeometry;

		// SSAO resources
		Ref<API::Core::Framebuffer> SSAOFramebuffer;
		Ref<API::Core::Framebuffer> SSAOBlurFramebuffer;
		Ref<API::Advanced::SSAO> SSAOProcessor;
	};
}