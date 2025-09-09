#pragma once

#include "3DCast.h"
#include "3DCast/Event/MouseEvent.h"

#include "GUI/Panels/SceneHierarchyPanel.h"
#include "GUI/Panels/SkyboxPanel.h"
#include "GUI/Panels/TerminalPanel.h"

#include "Layer/Blocks/RasterizationViewport.h"
#include "Layer/Blocks/PBRViewport.h"

class EditorLayer final : public Cast::Layer
{
public:
	EditorLayer();
	~EditorLayer() override = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Cast::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Cast::Event& e) override;

private:
	void Render();

	bool OnMouseScrolled(const Cast::MouseScrolledEvent& e);
	bool OnMousePressed(const Cast::MouseButtonPressedEvent& e);

	void CloseScene();

private:
	float DeltaTime = 0.0f;
	float CameraSpeed = 4.f;

	Runtime::PBRViewport ViewportPbr;
	Runtime::RasterizationViewport ViewportRasterization;

	Cast::Serialization::SceneSerializer Serializer;

	// Panels
	Runtime::GUI::SceneHierarchyPanel SceneHierarchyPanel;
	Runtime::GUI::SkyboxPanel SkyboxPanel;
	Runtime::GUI::TerminalPanel TerminalPanel{};
};
