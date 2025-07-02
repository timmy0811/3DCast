#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Renderer/Renderer.h"

#include <imgui.h>

namespace Cast::Component {
	struct CameraComponent final : public Component
	{
#pragma region DATA
		Renderer::Camera Camera;
#pragma endregion

#pragma region CONSTRUCTOR
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		explicit CameraComponent(const Renderer::Camera& camera)
			: Camera(camera) {}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Camera; }
		static inline std::string GetName() { return "Camera"; }

		UIResponse OnImGuiRender() override {
			const bool isOpen = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Camera"))
				return { UIResponse::Code::Remove, Type::Camera };

			if (isOpen) {
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

			return {};
		}
#pragma endregion
	};
}