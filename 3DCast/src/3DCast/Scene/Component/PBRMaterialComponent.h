#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>

namespace Cast::Component {
	struct PBRMaterialComponent : public Component
	{
#pragma region DATA
		bool placeholder;
#pragma endregion

#pragma region CONSTRUCTOR
		PBRMaterialComponent() = default;
		PBRMaterialComponent(const PBRMaterialComponent&) = default;
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline const Type GetType() { return Type::PBRMat; }
		static inline std::string GetName() { return "PBR Material"; }

		virtual UIResponse OnImGuiRender() override {
			bool isOpen = ImGui::CollapsingHeader("PBR Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##PBRMat"))
				return { UIResponse::Code::Remove, Type::PBRMat };

			if (isOpen) {
				ImGui::Text("No content to show here :)");
			}

			return {};
		}
#pragma endregion
	};
}