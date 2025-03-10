#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct CameraComponent : public Component
	{
		Renderer::Camera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Renderer::Camera& camera)
			: Camera(camera) {}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3 position = Camera.GetPosition();
				glm::vec3 rotation = Camera.GetRotation();

				ImGui::Text("Position:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				ImGui::DragFloat3("##Position", &position.x, 0.1f);

				ImGui::Text("Rotation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f);

				Camera.SetPosition(position);
				Camera.SetRotation(rotation);
			}
		}
	};
}