#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>
#include <string>
#include <vector>
#include <utility>

namespace Cast::Component
{
	struct TagComponent final : public Component
	{
#pragma region DATA
		std::string Tag;
		std::string Icon;
		bool IconHasManuallyAssigned = false;
		constexpr static size_t MAX_TAG_SIZE = 96;

		// Available icons for entity tagging
		inline static const std::vector<std::pair<std::string, std::string>> AvailableIcons = {
			{ICON_FA_CUBE, "Object"},
			{ICON_FA_CUBES, "Objects"},
			{ICON_FA_VIDEO, "Camera"},
			{ICON_FA_DRAW_POLYGON, "Mesh"},
			{ICON_FA_LIGHTBULB, "Pointlight"},
			{ICON_FA_SUN, "Dir-Light"},
			{ICON_FA_BURST, "Spotlight"},
			{ICON_FA_CIRCLE_HALF_STROKE, "Material"},
			{ICON_FA_CODE, "Shader"},
			{ICON_FA_APPLE_WHOLE, "Apple"},
			{ICON_FA_GEAR, "Gear"},
			{ICON_FA_ARROW_POINTER, "Cursor"},
			{ICON_FA_HEART, "Heart"},
			{ICON_FA_BOLT, "Bolt"},
			{ICON_FA_CLOUD, "Cloud"},
			{ICON_FA_LAYER_GROUP, "Layer Group"},
			{ICON_FA_SHAPES, "Primitive"}
		};
#pragma endregion

#pragma region CONSTRUCTOR
		TagComponent() { Tag.reserve(MAX_TAG_SIZE); };
		TagComponent(const TagComponent&) = default;

		explicit TagComponent(const std::string& tag, const std::string& icon = ICON_FA_CUBE){
			Tag = tag.empty() ? "Untagged" : tag;
			Tag.reserve(MAX_TAG_SIZE);

			Icon = icon;
		}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Tag; }
		static inline std::string GetName() { return "Tag"; }

		UIResponse OnImGuiRender() override
		{
			ImGui::Text(ICON_FA_TAG " Name");
			ImGui::SameLine(GUIWIN_WSIXTH);
			ImGui::SetNextItemWidth(GUIWIN_WSIXTH * 2.f - GUIWIN_ELEMENT_PADDING);
			ImGui::InputText("##Tag", Tag.data(), MAX_TAG_SIZE);

			ImGui::SameLine(GUIWIN_WSIXTH * 3.f);
			ImGui::Text(ICON_FA_ICONS " Icon");
			ImGui::SameLine(GUIWIN_WSIXTH * 4.f);
			ImGui::SetNextItemWidth(GUIWIN_WSIXTH * 2.f - GUIWIN_ENDELEMENT_PADDING);

			std::string currentIconPreview = Icon;
			for (const auto& [icon, name] : AvailableIcons) {
				if (icon == Icon) {
					currentIconPreview = icon + " " + name;
					break;
				}
			}

			if (ImGui::BeginCombo("##IconDropdown", currentIconPreview.c_str())) {
				for (const auto& [icon, name] : AvailableIcons) {
					const bool isSelected = (Icon == icon);
					const std::string label = icon + " " + name;
					if (ImGui::Selectable(label.c_str(), isSelected)) {
						Icon = icon;
						IconHasManuallyAssigned = true;
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			return {};
		}
#pragma endregion
	};
}