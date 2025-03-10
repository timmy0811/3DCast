#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct TransformComponent : public Component
	{
		bool isValid;
		size_t bufferPosition;
		unsigned short bufferIndex;
		glm::mat4 Transform{ 1.0f };
		glm::vec3 translation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::vec3 rotation{ 0.0f };
		Cast::Ref<API::Core::Buffer> transformRegistry;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		}

		bool Register(Cast::Ref<API::Core::Buffer> transformRegistry)
		{
			this->transformRegistry = transformRegistry;

			if (transformRegistry) {
				bufferPosition = transformRegistry->GetSize();
				transformRegistry->AddData(&Transform, sizeof(glm::mat4));
				bufferIndex = (unsigned short)(bufferPosition / sizeof(glm::mat4));
				isValid = true;
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

		virtual void OnImGuiRender() override
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
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
					transformRegistry->AddData(&Transform, sizeof(glm::mat4), (int)bufferPosition);
			}
		}
	};
}