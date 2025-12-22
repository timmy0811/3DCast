#pragma once

#include <3DCast.h>

namespace Runtime {
	struct View
	{
		float ParallaxScale = 0.02f;
		bool SSAOEnabled = true;
		float SSAOAffectness = 0.6f;
	};

	struct SharedEditorData
	{
		Cast::Optional<Cast::Renderer::Camera*> ActiveCamera;
		Cast::Ref<Cast::Entity>* SelectedEntity = nullptr;
		Cast::Renderer::Skybox Skybox{};
		Cast::Entity* EnvironmentLightEntity = nullptr;

		View ViewSettings;
	};

	extern SharedEditorData EditorContext;
}
