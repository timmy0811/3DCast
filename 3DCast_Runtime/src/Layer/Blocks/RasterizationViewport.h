#pragma once

#include "Viewport.h"

namespace Runtime {
	class RasterizationViewport : public Viewport
	{
	public:
		RasterizationViewport() = default;
		RasterizationViewport(Cast::Layer* parent);
		virtual ~RasterizationViewport() = default;

		void Init() override;
		void Destroy() override;

		void OnUpdate(Cast::Timestep ts) override;
		void OnRender() override;

		void OnImGuiRender() override;

		void OnEvent(Cast::Event& e) override;

	private:
		void RenderGeometryPass();
		void RenderLightingPass();
		void RenderForwardPass();

		void CompileShaders();

		// Event Handlers
		bool OnMouseMoved(Cast::MouseMovedEvent& e);

	private:
		Cast::Renderer::RenderPipelineObjectData RenderPipelineData;

		bool IsInitFrame = true;
		bool IsCameraRotating = false;
		glm::vec2 RelativeMousePosition = { 0.0f, 0.f };
	};
}
