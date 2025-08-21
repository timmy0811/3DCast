#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>

namespace Cast::Component {
	struct ShaderComponent final : public Component
	{
#pragma region DATA
		std::string Identifier;
		Ref<API::Core::Shader> Shader;
#pragma endregion

#pragma region CONSTRUCTOR
		ShaderComponent() = default;
		ShaderComponent(const ShaderComponent&) = default;
		ShaderComponent(const std::string& path_vertex, const std::string& path_frag, const std::string& id)
			: Identifier(id) {
			Shader.reset(API::Core::Shader::Create(path_vertex, path_frag));
		}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Shader; }
		static inline std::string GetName() { return "Shader"; }

		UIResponse OnImGuiRender() override {
			const bool isOpen = ImGui::CollapsingHeader(ICON_FA_CODE "  Shader", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##Shader"))
				return { UIResponse::Code::Remove, Type::Shader };

			if (isOpen) {
				ImGui::Text("Custom Shaders coming soon :)");
				ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
			}

			return {};
		}
#pragma endregion
	};
}