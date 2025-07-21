#pragma once

#include <3DCast.h>

namespace Runtime {
	struct View
	{
		float ParallaxScale = 0.02f;
	};

	struct SharedEditorData
	{
		Cast::Ref<Cast::Renderer::Camera> ActiveCamera;
		Cast::Renderer::Skybox Skybox{};

		View ViewSettings;
	};

	extern SharedEditorData EditorContext;
}
