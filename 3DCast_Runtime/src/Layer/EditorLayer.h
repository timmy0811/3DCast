#pragma once

#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Layer/Layer.h"
#include "3DCast/Event/ApplicationEvent.h"
#include "3DCast/Event/MouseEvent.h"
#include "3DCast/Renderer/Camera/Camera.h"

#include "API/core/Framebuffer.h"

#include "GUI/Panels/SceneHierarchyPanel.h"

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
	bool OnWindowResize(Cast::WindowResizeEvent& e);
	bool OnMouseMoved(Cast::MouseMovedEvent& e);
	bool OnMouseScrolled(Cast::MouseScrolledEvent& e);
	void Render();

private:
	inline static float CameraSpeed = 0.03f;
	Cast::Ref<API::Core::Framebuffer> Framebuffer;
	Cast::Ref<Cast::Scene> ActiveScene;

	Cast::Ref<Cast::Renderer::Camera> ActiveCamera;
	Cast::Entity CubeEntity;

	bool ViewportFocused = false;
	bool ViewportHovered = false;
	glm::vec2 ViewportSize = { 0.0f, 0.0f };
	glm::vec2 LastMousePosition = { 0.0f, 0.f };
	bool IsInitFrame = true;

	// Panels
	Runtime::GUI::SceneHierarchyPanel SceneHierarchyPanel;

	struct KeyState
	{
		bool isLMBPressed = false;
	} CurrentKeyState;
};
