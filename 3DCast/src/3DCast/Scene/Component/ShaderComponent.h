#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>

namespace Cast::Component {
	struct ShaderComponent : public Component
	{
#pragma region DATA
		std::string Identifier;
		Cast::Ref<API::Core::Shader> Shader;
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
		static inline const Type GetType() { return Type::Shader; }
		static inline std::string GetName() { return "Shader"; }

		virtual UIResponse OnImGuiRender() override {
			bool isOpen = ImGui::CollapsingHeader("Shader", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Shader"))
				return { UIResponse::Code::Remove, Type::Shader };

			if (isOpen) {
				ImGui::Text("No content to show here :)");
			}

			return {};
		}
#pragma endregion
	};
}