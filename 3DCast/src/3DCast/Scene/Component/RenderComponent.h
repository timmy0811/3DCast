#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>

namespace Cast::Component {
	struct RasterizableComponent final : public Component
	{
#pragma region DATA
		bool Renderable{ true };
#pragma endregion

#pragma region CONSTRUCTOR
		RasterizableComponent() = default;
		RasterizableComponent(const RasterizableComponent&) = default;

		explicit RasterizableComponent(const bool enable)
			: Renderable(enable) {}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Raster; }
		static inline std::string GetName() { return "Rasterizable"; }

		UIResponse OnImGuiRender() override {
			const bool isOpen = ImGui::CollapsingHeader("Rasterizable", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Rasterizable"))
				return { UIResponse::Code::Remove, Type::Raster };

			if (isOpen) {
				ImGui::Text("No content to show here :)");
			}

			return {};
		}
#pragma endregion
	};

	struct PBRComponent final : public Component
	{
#pragma region DATA
		bool Renderable{ true };
#pragma endregion

#pragma region CONSTRUCTOR
		PBRComponent() = default;
		PBRComponent(const PBRComponent&) = default;

		explicit PBRComponent(const bool enable)
			: Renderable(enable) {}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::PBR; }
		static inline std::string GetName() { return "PBR Renderable"; }

		UIResponse OnImGuiRender() override {
			const bool isOpen = ImGui::CollapsingHeader("PBR Renderable", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##PBR"))
				return { UIResponse::Code::Remove, Type::PBR };

			if (isOpen) {
				ImGui::Text("No content to show here :)");
			}

			return {};
		}
#pragma endregion
	};
}