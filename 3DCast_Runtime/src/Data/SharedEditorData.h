#pragma once

#include <3DCast.h>

namespace Runtime {
	enum class ViewportFilterMode
	{
		Nearest = 0,
		Linear = 1
	};

	struct View
	{
		// Parallax Mapping
		float ParallaxScale = 0.02f;

		// SSAO
		bool SSAOEnabled = true;
		float SSAOAffectness = 0.6f;

		// Shadow Mapping
		float ShadowBiasFactors[3] = { 1.0f, 2.0f, 10.0f };

		// Resize
		int ViewportWidth = 1920;
		int ViewportHeight = 1080;
		bool AdjustToWindowSize = true;
		ViewportFilterMode FilterMode = ViewportFilterMode::Linear;
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
