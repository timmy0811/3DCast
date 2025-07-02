#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include <imgui.h>
#include <string>

namespace Cast::Component
{
	struct TagComponent final : public Component
	{
#pragma region DATA
		std::string Tag;
		constexpr static size_t MAX_TAG_SIZE = 96;
#pragma endregion

#pragma region CONSTRUCTOR
		TagComponent() { Tag.reserve(MAX_TAG_SIZE); };
		TagComponent(const TagComponent&) = default;

		explicit TagComponent(const std::string& tag) {
			Tag = tag.empty() ? "Untagged" : tag;
			Tag.reserve(MAX_TAG_SIZE);
		}
#pragma endregion

#pragma region UTILITY
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Tag; }
		static inline std::string GetName() { return "Tag"; }

		UIResponse OnImGuiRender() override
		{
			ImGui::Text("Name:");
			ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
			ImGui::InputText("##Tag", Tag.data(), MAX_TAG_SIZE);

			return {};
		}
#pragma endregion
	};
}