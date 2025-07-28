#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtx/euler_angles.hpp>
#include <vendor/glm/gtx/matrix_decompose.hpp>

#include "3DCast/Scene/TransformRegistry.h"

#include <imgui.h>

#include "vendor/glm/gtc/type_ptr.hpp"

namespace Cast::Component
{
	struct TransformComponent final : public Component
	{
#pragma region DATA
		glm::mat4 Transform{1.0f};
		glm::vec3 translation{0.0f};
		glm::vec3 scale{1.0f};
		glm::vec3 rotation{0.0f};

		TransformRegistry* transformRegistry = nullptr;

		bool isRegistered = false;
		int transformRegistryIndex = -1;

#pragma endregion

#pragma region CONSTRUCTOR
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		explicit TransformComponent(const glm::mat4& transform)
			: Transform(transform)
		{
		}

		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale) *
				glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		}

		~TransformComponent() override
		{
			if (transformRegistry && isRegistered)
			{
				transformRegistry->InvalidateEntry(transformRegistryIndex);
				isRegistered = false;
			}
		}
#pragma endregion

#pragma region UTILITY
		bool Register(TransformRegistry* transformRegistry)
		{
			this->transformRegistry = transformRegistry;

			if (transformRegistry)
			{
				transformRegistryIndex = transformRegistry->RegisterTransform(&Transform);
				isRegistered = true;
				return true;
			}

			return false;
		}

		[[nodiscard]] inline glm::mat4 GetTransform() const
		{
			return Transform;
		}

		[[nodiscard]] inline float* GetTransformValuePtr()
		{
			return glm::value_ptr(Transform);
		}

		[[nodiscard]] inline glm::vec3 GetTranslation() const
		{
			return glm::vec3(Transform[3]);
		}

		[[nodiscard]] glm::vec3 GetScale() const
		{
			return {glm::length(glm::vec3(Transform[0])),
			                 glm::length(glm::vec3(Transform[1])),
			                 glm::length(glm::vec3(Transform[2]))};
		}

		[[nodiscard]] glm::vec3 GetRotation() const
		{
			const glm::vec3 scale = GetScale();

			const auto rotationMatrix = glm::mat3(
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

		void UpdateOnGPUMem() const
		{
			if (isRegistered)
				transformRegistry->EditTransform(transformRegistryIndex, &Transform);
		}
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Transform; }
		static inline std::string GetName() { return "Transform"; }

		UIResponse OnImGuiRender() override
		{
			const bool isOpen = ImGui::CollapsingHeader("Transform",
			                                      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Transform"))
				return {UIResponse::Code::Remove, Type::Camera};

			if (isOpen)
			{
				bool edited = false;
				ImGui::Text("Translation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Translation", &translation.x, 0.1f))
				{
					UpdateTransformMatrix();
					edited = true;
				}

				ImGui::Text("Scale:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Scale", &scale.x, 0.1f))
				{
					UpdateTransformMatrix();
					edited = true;
				}

				ImGui::Text("Rotation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f))
				{
					UpdateTransformMatrix();
					edited = true;
				}

				if (edited && transformRegistry)
					UpdateOnGPUMem();
			}

			return {};
		}
#pragma endregion
	};
}
