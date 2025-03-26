#pragma once

#include <3DCast.h>

namespace Runtime {
	struct View
	{
		float ParallaxScale = 0.00f;
	};

	struct SharedEditorData
	{
		Cast::Ref<Cast::Renderer::Camera> ActiveCamera;
		View ViewSettings;
	};

	extern SharedEditorData EditorContext;
}