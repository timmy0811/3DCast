#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>

#include "3DCast/Renderer/Camera/OrthographicCamera.h"
#include "3DCast/Renderer/Camera/PerspectiveCamera.h"

namespace Cast::Component {
	struct CameraComponent final : public Component
	{
#pragma region DATA
		Renderer::Camera* Camera;
		bool ownsCamera{ false };
#pragma endregion

#pragma region CONSTRUCTOR
		explicit CameraComponent(const Renderer::Camera::Type type = Renderer::Camera::Type::Perspective){
			switch (type) {
			case Renderer::Camera::Type::Perspective:
				Camera = new Renderer::PerspectiveCamera();
				break;
			case Renderer::Camera::Type::Orthographic:
				Camera = new Renderer::OrthographicCamera();
				break;
			default:
				Camera = new Renderer::PerspectiveCamera();
				break;
			}

			ownsCamera = true;
		}

		CameraComponent(const float fov, const float aspect, const float nearPlane = 0.1f, const float farPlane = 100.f) {
			Camera = new Renderer::PerspectiveCamera(fov, aspect, nearPlane, farPlane);
			ownsCamera = true;
		}

		CameraComponent(const float left, const float right, const float bottom, const float top, const float nearPlane = -1.f, const float farPlane = 1.f) {
			Camera = new Renderer::OrthographicCamera(left, right, bottom, top, nearPlane, farPlane);
			ownsCamera = true;
		}

		explicit CameraComponent(Renderer::Camera* camera)
			: Camera(camera) {}

		//CameraComponent(const CameraComponent&) = default;

		~CameraComponent() override
		{
			if (ownsCamera) {
				delete Camera;
			}
		}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Camera; }
		static inline std::string GetName() { return "Camera"; }

		UIResponse OnImGuiRender() override {
			const bool isOpen = ImGui::CollapsingHeader(ICON_FA_VIDEO "  Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##Camera"))
				return { UIResponse::Code::Remove, Type::Camera };

			if (isOpen) {
				glm::vec3 position = Camera->GetPosition();
				glm::vec3 rotation = Camera->GetRotation();

				ImGui::Text("Position");
				ImGui::SameLine(GUIWIN_WTHIRD);
				TWOTHIRD_INPUT_WIDTH
				ImGui::DragFloat3("##Position", &position.x, 0.1f);

				ImGui::Text("Rotation");
				ImGui::SameLine(GUIWIN_WTHIRD);
				TWOTHIRD_INPUT_WIDTH
				ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f);

				Camera->SetPosition(position);
				Camera->SetRotation(rotation);

				if (Camera->GetType() == Renderer::Camera::Type::Perspective) {
					if (auto* perspectiveCam = dynamic_cast<Renderer::PerspectiveCamera*>(Camera)) {
						float fov = perspectiveCam->GetFOV();
						ImGui::Text("FOV");
						ImGui::SameLine(GUIWIN_WTHIRD);
						TWOTHIRD_INPUT_WIDTH
						if (ImGui::DragFloat("##FOV", &fov, 1.0f, 5.0f, 175.0f, "%.1f°")) {
							perspectiveCam->SetFOV(fov);
						}
					}
				}
			}

			return {};
		}
#pragma endregion
	};
}
