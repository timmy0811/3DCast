#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Scene/Component/MeshComponent.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtx/euler_angles.hpp>
#include <vendor/glm/gtx/matrix_decompose.hpp>

#include "../Registry/TransformRegistry.h"
#include "3DCast/Misc/Structs.h"

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

		BoundingBox BBox{};

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
			BBox.SetCenter(transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}

		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale) *
				glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);

			BBox.SetCenter(translation);
		}

		TransformComponent(TransformComponent&& other) noexcept
		: Transform(other.Transform), translation(other.translation),
		  scale(other.scale), rotation(other.rotation),
		  transformRegistry(other.transformRegistry),
		  isRegistered(other.isRegistered),
		  transformRegistryIndex(other.transformRegistryIndex)
		{
			if (transformRegistry && isRegistered)
			{
				transformRegistry->Edit(transformRegistryIndex, &Transform);
			}

			other.isRegistered = false;
			other.transformRegistryIndex = -1;
		}

		TransformComponent& operator=(TransformComponent&& other) noexcept
		{
			if (this != &other)
			{
				if (transformRegistry && isRegistered)
				{
					transformRegistry->InvalidateEntry(transformRegistryIndex);
				}

				Transform = other.Transform;
				translation = other.translation;
				scale = other.scale;
				rotation = other.rotation;
				transformRegistry = other.transformRegistry;
				isRegistered = other.isRegistered;
				transformRegistryIndex = other.transformRegistryIndex;

				if (transformRegistry && isRegistered)
				{
					transformRegistry->Edit(transformRegistryIndex, &Transform);
				}

				other.isRegistered = false;
				other.transformRegistryIndex = -1;
			}

			return *this;
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
				transformRegistryIndex = transformRegistry->Register(&Transform);
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
			return glm::vec<3, float>(Transform[3]);
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

		void DecomposeTransformOnComponents()
		{
			translation = GetTranslation();
			scale = GetScale();
			rotation = GetRotation();

			UpdateBBox();
		}

		void UpdateBBox()
		{
			BBox.SetCenter(translation);

			constexpr float divVal = 1.f / 3.f;
			BBox.SetSize((scale.x + scale.y + scale.z) * divVal);
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
				transformRegistry->Edit(transformRegistryIndex, &Transform);
		}
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Transform; }
		static inline std::string GetName() { return "Transform"; }

		UIResponse OnImGuiRender() override
		{
			const bool isOpen = ImGui::CollapsingHeader(ICON_FA_CROP_SIMPLE "  Transform",
			                                      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##Transform"))
				return {UIResponse::Code::Remove, Type::Camera};

 		if (isOpen)
 		{
 			// Check if this transform belongs to a non-root mesh node (Intermediate or Leaf)
 			// If so, gray out the controls since the transform is not used for these nodes
 			bool isNonRootMeshNode = false;
 			if (EntityNode && EntityNode->HasComponent<MeshComponent>())
 			{
 				const auto& meshComp = EntityNode->GetComponent<MeshComponent>();
 				MeshNodeType nodeType = meshComp.GetNodeType();
 				isNonRootMeshNode = (nodeType == MeshNodeType::Intermediate || nodeType == MeshNodeType::Leaf);
 			}

 			if (isNonRootMeshNode)
 			{
 				ImGui::BeginDisabled(true);
 				ImGui::TextWrapped("Transform is controlled by the root model node.");
 			}

 			bool edited = false;
 			ImGui::Text("Translation");
 			ImGui::SameLine(GUIWIN_WTHIRD);
 			TWOTHIRD_INPUT_WIDTH
 			if (ImGui::DragFloat3("##Translation", &translation.x, 0.1f))
 			{
 				UpdateTransformMatrix();
 				edited = true;
 			}

 			ImGui::Text("Scale");
 			ImGui::SameLine(GUIWIN_WTHIRD);
 			TWOTHIRD_INPUT_WIDTH
 			if (ImGui::DragFloat3("##Scale", &scale.x, 0.1f))
 			{
 				UpdateTransformMatrix();
 				edited = true;
 			}

 			ImGui::Text("Rotation");
 			ImGui::SameLine(GUIWIN_WTHIRD);
 			TWOTHIRD_INPUT_WIDTH
 			if (ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f))
 			{
 				UpdateTransformMatrix();
 				edited = true;
 			}

 			if (isNonRootMeshNode)
 			{
 				ImGui::EndDisabled();
 			}

 			if (edited && transformRegistry)
 				UpdateOnGPUMem();

 			ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
 		}

			return {};
		}
#pragma endregion
	};
}
