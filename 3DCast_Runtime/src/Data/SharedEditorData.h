#pragma once

#include <3DCast.h>
#include <vendor/glm/glm.hpp>

namespace Runtime {
	enum class ViewportFilterMode
	{
		Nearest = 0,
		Linear = 1
	};

	struct FogSettings
	{
		bool Enabled = false;
		glm::vec3 ColorBottom = glm::vec3(0.5f, 0.6f, 0.7f);
		glm::vec3 ColorTop = glm::vec3(0.7f, 0.8f, 0.9f);
		float StartDistance = 10.0f;
		float EndDistance = 100.0f;
		float AngleMin = -30.0f;
		float AngleMax = 30.0f;
		float Density = 2.0f;
		bool UseExponential = true;
	};

	struct View
	{
		// Parallax Mapping
		float ParallaxScale = 0.02f;

		// SSAO
		bool SSAOEnabled = true;
		float SSAOAffectness = 0.6f;

		// Retro
		bool DitheringEnabled = false;
		float DitheringStrength = 1.0f;     // 0.0 - 2.0
		int DitheringColorDepth = 16;       // Color levels per channel
		float DitheringScale = 1.0f;        // Pattern scale (1.0 = pixel perfect)

		bool VertexSnappingEnabled = false;
		float VertexSnappingResolution = 160.0f;

		bool AffineTextureMappingEnabled = false;
		float AffineTextureStrength = 0.5f;

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
		FogSettings Fog;
	};

	extern SharedEditorData EditorContext;
}
