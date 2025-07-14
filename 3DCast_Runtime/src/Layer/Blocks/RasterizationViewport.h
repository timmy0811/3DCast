#pragma once

#include "Viewport.h"
#include "3DCast/Event/MouseEvent.h"

namespace Runtime
{
	class RasterizationViewport final : public Viewport
	{
	public:
		RasterizationViewport() = default;
		explicit RasterizationViewport(Cast::Layer* parent);
		~RasterizationViewport() override = default;

		void Init() override;
		void Destroy() override;

		inline void FindWindow() { SetPositionOnNextDraw = true; }

		void OnUpdate(Cast::Timestep ts, bool hasCameraChanged = false) override;
		void OnRender() override;

		void OnImGuiRender() override;

		void OnEvent(Cast::Event& e) override;

		void UpdateCameraUniforms();
	private:
		void RenderGeometryPass() const;
		void RenderLightingPass() const;
		void RenderForwardPass() const;

		static void CompileShaders();


		// Event Handlers
		bool OnMouseMoved(Cast::MouseMovedEvent& e);

	private:
		Cast::Renderer::RasterizationPipelineObjects PipelineData;

		bool Wireframe = false;
		bool SetPositionOnNextDraw = false;
		bool IsCameraInitFrame = true;
		bool IsCameraRotating = false;
		glm::vec2 RelativeMousePosition = {0.0f, 0.f};
	};
}
