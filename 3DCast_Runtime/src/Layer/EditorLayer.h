#pragma once

#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Layer/Layer.h"

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
	Cast::Ref<Cast::Scene> ActiveScene;
	Cast::Entity ActiveCamera;
	Cast::Entity CubeEntity;

	bool ViewportFocused = false;
	bool ViewportHovered = false;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
};
