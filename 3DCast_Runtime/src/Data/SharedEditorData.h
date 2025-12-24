#pragma once

#include <3DCast.h>

namespace Runtime {
	struct View
	{
		float ParallaxScale = 0.02f;
		bool SSAOEnabled = true;
		float SSAOAffectness = 0.6f;
		float ShadowBiasFactors[3] = { 1.0f, 2.0f, 10.0f };
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
