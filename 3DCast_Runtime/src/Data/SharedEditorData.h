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

		// Dithering
		bool DitheringEnabled = false;
		float DitheringStrength = 1.0f;     // 0.0 - 2.0
		int DitheringColorDepth = 16;       // Color levels per channel (PS1 = 32)
		float DitheringScale = 1.0f;        // Pattern scale (1.0 = pixel perfect)

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
