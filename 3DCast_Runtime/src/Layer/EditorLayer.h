#pragma once

#include "3DCast.h"

#include "GUI/Panels/SceneHierarchyPanel.h"

#include "Layer/Blocks/RasterizationViewport.h"
#include "Layer/Blocks/PBRViewport.h"

#include "vendor/glm/glm.hpp"

class EditorLayer : public Cast::Layer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Cast::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Cast::Event& e) override;

private:
	void Render();

	// Temporary
	void SampleContent();

	// Event Handlers
	bool OnMouseScrolled(Cast::MouseScrolledEvent& e);

private:
	float DeltaTime = 0.0f;

	Runtime::PBRViewport ViewportPbr;
	Runtime::RasterizationViewport ViewportRasterization;

	float CameraSpeed = 4.f;

	Cast::Entity CubeEntity;

	// Panels
	Runtime::GUI::SceneHierarchyPanel SceneHierarchyPanel;
};
