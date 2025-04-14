#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <Vendor/glm/glm.hpp>
#include <vendor/glm/gtx/euler_angles.hpp>
#include <Vendor/glm/gtx/matrix_decompose.hpp>

#include "3DCast/Scene/TransformRegistry.h"

namespace Cast::Component {
	struct TransformComponent : public Component
	{
#pragma region DATA
		glm::mat4 Transform{ 1.0f };
		glm::vec3 translation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::vec3 rotation{ 0.0f };

		TransformRegistry* transformRegistry = nullptr;

		bool isRegistered = false;
		uid transformRegistryKey = UID::None();

#pragma endregion

#pragma region CONSTRUCTOR
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		}

		~TransformComponent() {
			if (transformRegistry && isRegistered) {
				transformRegistry->InvalidateEntry(transformRegistryKey);
				isRegistered = false;
			}
		}

#pragma endregion

#pragma region UTILITY
		int GetRegistryPosition() {
			if (transformRegistry && isRegistered) {
				return transformRegistry->GetPosition(transformRegistryKey);
			}
			return -1;
		}

		bool Register(TransformRegistry* transformRegistry)
		{
			this->transformRegistry = transformRegistry;

			if (transformRegistry) {
				transformRegistryKey = transformRegistry->RegisterTransform(&Transform);
				isRegistered = true;
				return true;
			}

			return false;
		}

		inline glm::mat4 GetTransform() const
		{
			return Transform;
		}

		inline glm::vec3 GetTranslation() const
		{
			return glm::vec3(Transform[3]);
		}

		glm::vec3 GetScale() const
		{
			return glm::vec3(glm::length(glm::vec3(Transform[0])),
				glm::length(glm::vec3(Transform[1])),
				glm::length(glm::vec3(Transform[2])));
		}

		glm::vec3 GetRotation() const
		{
			glm::vec3 scale = GetScale();

			glm::mat3 rotationMatrix = glm::mat3(
				Transform[0] / scale.x,
				Transform[1] / scale.y,
				Transform[2] / scale.z
			);

			return glm::degrees(glm::eulerAngles(glm::quat_cast(rotationMatrix)));
		}

		void UpdateTransformMatrix()
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) *
				glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)) *
				glm::scale(glm::mat4(1.0f), scale);
		}
#pragma endregion

#pragma region OVERRIDE
		static inline const Type GetType() { return Type::Transform; }
		static inline std::string GetName() { return "Transform"; }

		virtual UIResponse OnImGuiRender() override
		{
			bool isOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Transform"))
				return { UIResponse::Code::Remove, Type::Camera };

			if (isOpen)
			{
				ImGui::Text("Translation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Translation", &translation.x, 0.1f))
					UpdateTransformMatrix();

				ImGui::Text("Scale:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Scale", &scale.x, 0.1f))
					UpdateTransformMatrix();

				ImGui::Text("Rotation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f))
					UpdateTransformMatrix();

				if (transformRegistry)
					transformRegistry->EditTransform(transformRegistryKey, &Transform);
			}

			return {};
		}
#pragma endregion
	};
}