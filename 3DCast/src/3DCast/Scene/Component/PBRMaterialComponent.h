#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>

namespace Cast::Component {
	struct PBRMaterialComponent final : public Component
	{
#pragma region DATA
		bool placeholder = false;
#pragma endregion

#pragma region CONSTRUCTOR
		PBRMaterialComponent() = default;
		PBRMaterialComponent(const PBRMaterialComponent&) = default;
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::PBRMat; }
		static inline std::string GetName() { return "PBR Material"; }

		UIResponse OnImGuiRender() override {
			const bool isOpen = ImGui::CollapsingHeader("PBR Material", ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##PBRMAT"))
				return { UIResponse::Code::Remove, Type::PBRMat };

			if (isOpen) {
				ImGui::Text("No content to show here :)");
				ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
			}

			return {};
		}
#pragma endregion
	};
}