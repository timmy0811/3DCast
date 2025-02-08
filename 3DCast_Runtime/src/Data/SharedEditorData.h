#pragma once

#include <3DCast.h>

namespace Runtime {
	struct SharedEditorData
	{
		Cast::Ref<Cast::Scene> ActiveScene;
		Cast::Ref<Cast::Renderer::Camera> ActiveCamera;
	};

	extern SharedEditorData EditorContext;
}